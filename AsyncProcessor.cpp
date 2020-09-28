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
    , mVisitedURLs(aVisitedURLs)
    , mDeque(aDeque)
    , mList(aList) {}

std::vector<std::string> AsyncProcessor::process(std::vector<std::string> urls)
{
	if (urls.size() > std::thread::hardware_concurrency())
		throw AsyncProcessorException("Wrong number of tasks");

	std::vector<std::future<ProcessedInfo>> futures;
	std::vector<std::string> foundURLs;

	for (const auto& url : urls)
        futures.push_back(std::async(std::launch::async, [this, &url] {return processor(QString::fromStdString(url)); }));

    QEventLoop loop;
    int counter = 1;

    connect(this, &AsyncProcessor::signalThreadFinished, [&] {if (counter++ == urls.size()) loop.quit(); });

    loop.exec();
	for (auto &future : futures)
	{
        const auto currentProccedData = future.get();
        const auto currentURLs = currentProccedData.foundURLs;
        mList.append({currentProccedData.currentURL, currentProccedData.currentStatus});
		foundURLs.insert(foundURLs.end(), currentURLs.begin(), currentURLs.end());
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
        return { {}, currentUrl, URLStatus::Error };
    }

    bool isTextFound = str.find(mConfig.textToFind.toStdString()) != std::string::npos;

    while (std::regex_search(str, url_match_result, url_regex))
    {
        if (mVisitedURLs.size() < mConfig.maxNumberOfScanningURLs)
        {
            if (mVisitedURLs.find(url_match_result.str()) == mVisitedURLs.end())
            {
                foundURLs.push_back(url_match_result.str());
               // std::lock_guard<std::mutex> lck(mMutex);
                mVisitedURLs.insert(url_match_result.str());
                std::cout << url_match_result.str() << std::endl;
            }
            str = url_match_result.suffix();
        }
        else
            break;
    }

    emit signalThreadFinished();
    return { foundURLs, currentUrl, isTextFound ? URLStatus::TextFound : URLStatus::TextNotFound };
}

AsyncProcessor::~AsyncProcessor() {}