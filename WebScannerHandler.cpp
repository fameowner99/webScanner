#include "WebScannerHandler.h"
#include <iostream>
#include <QNetworkAccessManager>

#include <QEventLoop>
#include "AsyncProcessor.h"
#include <future>
#include "ButtonsState.h"
WebScannerHandler::WebScannerHandler()
{
    qRegisterMetaType<QList<QPair<QString, URLStatus>>>("QList<QPair<QString,URLStatus>>");
    qRegisterMetaType<ButtonsState>("ButtonsState");

}

WebScannerHandler::~WebScannerHandler() {}

void WebScannerHandler::setConfigurations(const ScanningConfiguration& aConfig)
{
	mConfig = aConfig;
}

void WebScannerHandler::startScanning()
{
	std::cout << "START" << std::endl;;
    mDeque.clear();
    mUniqueURLs.clear();
    mList.clear();
    mDeque.push_back(mConfig.url.toStdString());
    mUniqueURLs.insert(mConfig.url.toStdString());
    emit signalButtonsStatusChanged(ButtonsState::Running);
    
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    QEventLoop loop;


    auto f = std::async(std::launch::async, [&]
    {
      
        while (!mDeque.empty())
        {
            QEventLoop pauseLoop;

            connect(this, &WebScannerHandler::signalResume, [&pauseLoop] {pauseLoop.quit(); });
            if (mIsPaused)
                pauseLoop.exec();

            AsyncProcessor ap(mConfig, mUniqueURLs, mDeque, mList);
            std::vector<std::string> scanningUrls;

            for (int i = 0; i < mConfig.maxNumberOfThreads; ++i)
            {
                if (mDeque.empty())
                    break;

                scanningUrls.push_back(mDeque.front());
                mDeque.pop_front();
            }

            const auto foundURL = ap.process(scanningUrls);
            mDeque.insert(mDeque.end(), foundURL.begin(), foundURL.end());
            updateList();
        }
        emit signalFinished();
        updateList();
    });

    connect(this, &WebScannerHandler::signalFinished, [&loop] {loop.quit(); });
    loop.exec();

    f.get();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
    std::cout << "THE END!__________";

    emit signalButtonsStatusChanged(ButtonsState::Finished);
    mIsPaused = false;
}

void WebScannerHandler::stopScanning()
{
	std::cout << "STOP" << std::endl;;
    if (mIsPaused)
        emit signalResume();
    mDeque.clear();
}

void WebScannerHandler::pauseScanning()
{
	std::cout << "PAUSE" << std::endl;
    mIsPaused = !mIsPaused;
    
    if (!mIsPaused)
        emit signalResume();
}

void WebScannerHandler::updateList()
{
    QList<QPair<QString, URLStatus>> list;
    for (const auto& q : mDeque)
        list.append({ QString::fromStdString(q) ,URLStatus::Queue });
    list.append(mList);
    emit signalListUpdated(list);
}