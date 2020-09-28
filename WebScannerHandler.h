#ifndef WebScannerHandler_h

#define WebScannerHandler_h

#pragma once

#include <QString>
#include <QObject>
#include <chrono>

#include <set>
#include <deque>
#include "URLStatus.h"
#include "ScanningConfiguration.h"
#include "ButtonsState.h"

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
	void signalListUpdated(const QList<QPair<QString, URLStatus>> list);
	void signalResume();
	void signalButtonsStatusChanged(ButtonsState);

private:
	void handleReply(const QString& reply, const QString& currentUrl);
	void updateList();

private:
	ScanningConfiguration mConfig;
	std::set<std::string> mVisitedURLs;
	std::deque<std::string> mDeque;
	QList<QPair<QString, URLStatus>> mList;
	bool mIsPaused = false;

};


#endif