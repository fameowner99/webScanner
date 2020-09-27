#ifndef WebScannerHandler_h

#define WebScannerHandler_h

#pragma once

#include <QString>
#include <QObject>

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

private:


private:
	ScanningConfiguration mConfig;
};

#endif