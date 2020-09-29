#include "AsyncProcessor.h"
#include <thread>
#include <future>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "URLStatus.h"
#include <QEventLoop>
#include <regex>
#include <iostream>

AsyncProcessor::AsyncProcessor(ScanningConfiguration& aConfig,
    std::set<std::string>& aVisitedURLs,
    std::deque<std::string>& aDeque,
    QList<QPair<QString, URLStatus>>& aList)
    : mConfig(aConfig)
    , mUniqueURLs(aVisitedURLs)
    , mDeque(aDeque)
    , mList(aList) {}

AsyncProcessor::~AsyncProcessor() {}

std::vector<std::string> AsyncProcessor::process(std::vector<std::string> urls)
{
	std::vector<std::future<ProcessedInfo>> futures;
	std::vector<std::string> foundURLs;

	for (const auto& url : urls)
        futures.push_back(std::async(std::launch::async, [this, &url] {return processor(QString::fromStdString(url)); }));

    QEventLoop loop;
    int workCounter = 1;

    connect(this, &AsyncProcessor::signalThreadFinished, [&] {if (workCounter++ == urls.size()) loop.quit(); });

    loop.exec();
	for (auto &future : futures)
	{
        const auto currentProccedData = future.get();
        const auto currentURLs = currentProccedData.foundURLs;
        mList.append({currentProccedData.currentURL, currentProccedData.currentStatus});
        std::cout << currentProccedData.currentURL.toStdString() << std::endl;
        for (const auto& foundURL : currentURLs)
        {
            if (mUniqueURLs.size() < mConfig.maxNumberOfScanningURLs)
            {
                if (mUniqueURLs.find(foundURL) == mUniqueURLs.end())
                {
                    mUniqueURLs.insert(foundURL);
                    foundURLs.push_back(foundURL);              
                }
            }
            else
                break;
        }
	}

	return foundURLs;
}

ProcessedInfo AsyncProcessor::processor(const QString url)
{
    QNetworkAccessManager* manager = new QNetworkAccessManager();
    QEventLoop loop;
    QString answer, currentURL;

    QObject::connect(manager, &QNetworkAccessManager::finished,
        this, [&loop, &answer, &currentURL](QNetworkReply* reply)
        {
            // if redirect than replay answer is empty
            // to fix this need to do second get request
            // error if redirect for now.
             if (reply->error() == QNetworkReply::NoError)
             {
                 int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                 if (statusCode == 301)
                 {
                     QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
                 }
             }

            answer = reply->readAll();
            currentURL = reply->url().toString();
            loop.quit();
        }
    );

    manager->get(QNetworkRequest(QUrl(url)));
    loop.exec();
    return handleReply(answer, currentURL);
}

ProcessedInfo  AsyncProcessor::handleReply(const QString& reply, const QString& currentUrl)
{
    //std::regex url_regex(R"((http|ftp|https)://([\w_-]+(?:(?:\.[\w_-]+)+))([\w.,@?^=%&:/~+#-]*[\w@?^=%&/~+#-])?)");
    // file for local test
    // std::regex url_regex(R"((file)://(.*)?)");
    std::regex url_regex(R"((http|https)://([\w_-]+(?:(?:\.[\w_-]+)+))([\w.,@?^=%&:/~+#-]*[\w@?^=%&/~+#-])?)");
    std::smatch url_match_result;
    std::string str = reply.toStdString();
    std::vector<std::string> foundURLs;

    if (str.empty())
    {
        emit signalThreadFinished();
        //std::cout << currentUrl.toStdString() <<  " - replay is empty! Redirection possible." << std::endl;
        return { {}, currentUrl, URLStatus::Error };
    }

    bool isTextFound = str.find(mConfig.textToFind.toStdString()) != std::string::npos;

    while (std::regex_search(str, url_match_result, url_regex))
    {
        foundURLs.push_back(url_match_result.str());
        str = url_match_result.suffix();
    }

    emit signalThreadFinished();
    return { foundURLs, currentUrl, isTextFound ? URLStatus::TextFound : URLStatus::TextNotFound };
}