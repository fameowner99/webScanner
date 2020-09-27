#ifndef WebScannerHandler_h

#define WebScannerHandler_h

#pragma once

#include <QString>
#include <QObject>
#include <QNetworkAccessManager>
#include <unordered_map>
#include <queue>

struct ScanningConfiguration
{
	QString url;
	QString textToFind;
	size_t	maxNumberOfThreads;
	size_t	maxNumberOfScanningURLs;
};

class WebScannerHandler : public QObject
{
	Q_OBJECT

public:
	WebScannerHandler();
	~WebScannerHandler();
	void setConfigurations(const ScanningConfiguration& aConfig);
	void startScanning();
	void stopScanning();
	void pauseScanning();
		

signals:
	void signalFinished();
	void signalReplyHandled();

private:
	void handleReply(const QString& reply, const QString& currentUrl);

private:
	ScanningConfiguration mConfig;
	std::vector<QNetworkAccessManager*> mManagers;
	std::unordered_map<std::string, bool> mVisitedURLs;
	std::queue<std::string> mQueue;

};

#endif