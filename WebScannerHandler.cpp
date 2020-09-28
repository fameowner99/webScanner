#include "WebScannerHandler.h"
#include <iostream>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <regex>
#include <QEventLoop>

WebScannerHandler::WebScannerHandler()
{
    mManagers.push_back(new QNetworkAccessManager());
    QObject::connect(mManagers[0], &QNetworkAccessManager::finished,
        this, [=](QNetworkReply* reply) {
            if (reply->error())
            {
                mList.append({ reply->url().toString(), URLStatus::Error });
                updateList();
                emit signalReplyHandled();
                return;
            }
            QString answer = reply->readAll();
           
            handleReply(answer, reply->url().toString());

            emit signalReplyHandled();
        }
    );
}

WebScannerHandler::~WebScannerHandler() {}

void WebScannerHandler::setConfigurations(const ScanningConfiguration& aConfig)
{
	mConfig = aConfig;
}

void WebScannerHandler::startScanning()
{
	std::cout << "START" << std::endl;;
    mDeque = {};
    mVisitedURLs = {};
    mList = {};

    mManagers[0]->get(QNetworkRequest(QUrl(mConfig.url)));
 
    
    QEventLoop loop;

    auto connection = connect(this, &WebScannerHandler::signalReplyHandled, [&]
    {
        if (mDeque.empty())
        {
            loop.quit();
            return;
        }
        const auto url = mDeque.front();
        mDeque.pop_front();
        mManagers[0]->get(QNetworkRequest(QUrl(QString::fromStdString(url))));
    });

    loop.exec();

    disconnect(connection);

    std::cout << "THE END!__________";
}

void WebScannerHandler::stopScanning()
{
	std::cout << "STOP" << std::endl;;
}

void WebScannerHandler::pauseScanning()
{
	std::cout << "PAUSE" << std::endl;
}

void WebScannerHandler::handleReply(const QString& reply, const QString &currentUrl)
{
    //std::regex url_regex(R"((http|ftp|https)://([\w_-]+(?:(?:\.[\w_-]+)+))([\w.,@?^=%&:/~+#-]*[\w@?^=%&/~+#-])?)");
    // file for local test
    // std::regex url_regex(R"((file)://(.*)?)");
    std::regex url_regex(R"((http)://([\w_-]+(?:(?:\.[\w_-]+)+))([\w.,@?^=%&:/~+#-]*[\w@?^=%&/~+#-])?)");
    std::smatch url_match_result;
    std::string str = reply.toStdString();

    if (str.empty())
    {
        mList.append({ currentUrl, URLStatus::Error });
        updateList();
        return;
    }
    auto tmp = currentUrl.toStdString();
    bool isTextFound = str.find(mConfig.textToFind.toStdString()) != std::string::npos;
    mList.append({ currentUrl, isTextFound ?  URLStatus::TextFound : URLStatus::TextNotFound });

    updateList();

    while (std::regex_search(str, url_match_result, url_regex))
    {
        if (mVisitedURLs.size() < mConfig.maxNumberOfScanningURLs)
        {
            if (mVisitedURLs.find(url_match_result.str()) == mVisitedURLs.end())
            {
                mDeque.push_back(url_match_result.str());
                mVisitedURLs.insert(url_match_result.str());
                std::cout << url_match_result.str() << std::endl;
            }
            str = url_match_result.suffix();
        }
        else
            break;
    }
}

void WebScannerHandler::updateList()
{
    QList<QPair<QString, URLStatus>> list;
    for (const auto& q : mDeque)
        list.append({ QString::fromStdString(q) ,URLStatus::Queue });
    list.append(mList);

    emit signalListUpdated(list);
}