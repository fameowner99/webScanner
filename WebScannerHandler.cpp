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
                qDebug() << reply->errorString();
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
    mQueue = {};
    mVisitedURLs = {};

    mManagers[0]->get(QNetworkRequest(QUrl(mConfig.url)));
 
    
    QEventLoop loop;

    auto connection = connect(this, &WebScannerHandler::signalReplyHandled, [&]
    {
        if (mQueue.empty())
        {
            loop.quit();
            return;
        }
        const auto url = mQueue.front();
        mQueue.pop();
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

    bool isTextFound = str.find(mConfig.textToFind.toStdString()) != std::string::npos;
    mVisitedURLs[currentUrl.toStdString()] = isTextFound;
    
    mList.append({ currentUrl, URLStatus::TextFound });
    emit signalListUpdated(mList);

    while (std::regex_search(str, url_match_result, url_regex))
    {
        if (mVisitedURLs.size() < mConfig.maxNumberOfScanningURLs)
        {
            mQueue.push(url_match_result.str());
            mVisitedURLs[url_match_result.str()] = false;
            std::cout << url_match_result.str() << std::endl;
            str = url_match_result.suffix();
        }
        else
            break;
    }
}