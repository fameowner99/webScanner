#ifndef WebScannerHandler_h

#define WebScannerHandler_h

#pragma once

#include <QString>
#include <QObject>
#include <QNetworkAccessManager>
#include <set>
#include <deque>

struct ScanningConfiguration
{
	QString url;
	QString textToFind;
	size_t	maxNumberOfThreads;
	size_t	maxNumberOfScanningURLs;
};

enum class URLStatus
{
	TextFound,
	TextNotFound,
	Queue,
	Scanning,
	Error
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
	void signalListUpdated(const QList<QPair<QString, URLStatus>> list);

private:
	void handleReply(const QString& reply, const QString& currentUrl);
	void updateList();

private:
	ScanningConfiguration mConfig;
	std::vector<QNetworkAccessManager*> mManagers;
	std::set<std::string> mVisitedURLs;
	std::deque<std::string> mDeque;
	QList<QPair<QString, URLStatus>> mList;

};


#endif