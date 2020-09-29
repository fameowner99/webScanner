#include "WebScannerHandler.h"
#include "AsyncProcessor.h"
#include "ButtonsState.h"

#include <iostream>
#include <QNetworkAccessManager>
#include <chrono>
#include <QEventLoop>
#include <future>

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
    
    auto task = [&]
    {
        std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
        QEventLoop pauseLoop;
        connect(this, &WebScannerHandler::signalResume, [&pauseLoop] {pauseLoop.quit(); });

        while (!mDeque.empty())
        {

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

            try
            {
                updateList(scanningUrls);
                const auto foundURL = ap.process(scanningUrls);
                mDeque.insert(mDeque.end(), foundURL.begin(), foundURL.end());
                
            }
            catch (AsyncProcessorException& e)
            {
                std::cout << e.what();
            }
            catch (...)
            {
                std::cout << "Some error!" << std::endl;
            }
        }
        emit signalFinished();
        updateList({});
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;
        std::cout << "THE END!__________";

        emit signalButtonsStatusChanged(ButtonsState::Finished);
        mIsPaused = false;
    };



    std::thread thread(task);
    thread.detach();
   
}

void WebScannerHandler::stopScanning()
{
	std::cout << "STOP" << std::endl;;

    mDeque.clear();
}

void WebScannerHandler::pauseScanning()
{
	std::cout << "PAUSE" << std::endl;
    mIsPaused = !mIsPaused;
    
    if (!mIsPaused)
        emit signalResume();

    emit signalButtonsStatusChanged(mIsPaused ? ButtonsState::Paused : ButtonsState::Running);
}

void WebScannerHandler::updateList(std::vector<std::string> scanningUrls)
{
    QList<QPair<QString, URLStatus>> list;
    for (const auto& s : scanningUrls)
        list.append({ QString::fromStdString(s) ,URLStatus::Scanning });
    for (const auto& q : mDeque)
        list.append({ QString::fromStdString(q) ,URLStatus::Queue });
    list.append(mList);
    emit signalListUpdated(list);
}