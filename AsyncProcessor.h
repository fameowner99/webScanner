#ifndef AsyncProcessor_h

#define AsyncProcessor_h

#include <vector>
#include <functional>
#include <string>
#include <exception>
#include <QObject>
#include <set>
#include <deque>
#include "ScanningConfiguration.h"
#include "URLStatus.h"
#include <mutex>

struct ProcessedInfo
{
	ProcessedInfo()
		: currentStatus(URLStatus::Unknown) {}
	ProcessedInfo(const std::vector<std::string>& aFoundURL, const QString &aCurrentURL, const URLStatus aCurrentStatus)
		: foundURLs(aFoundURL)
		, currentURL(aCurrentURL)
		, currentStatus(aCurrentStatus) {}

	std::vector<std::string> foundURLs;
	QString	currentURL;
	URLStatus currentStatus;

};

class AsyncProcessorException : public std::exception
{
public:
	AsyncProcessorException(const char* message)
		: std::exception(message) {}
};

class AsyncProcessor: public QObject
{
	Q_OBJECT
public:
	AsyncProcessor(ScanningConfiguration& aConfig,
		std::set<std::string>& aVisitedURLs,
		std::deque<std::string>& aDeque,
		QList<QPair<QString, URLStatus>>& aList);
	~AsyncProcessor();
	std::vector<std::string> process(std::vector<std::string> urls);

signals:
	void signalReplyHandled();
	void signalThreadFinished();

private:

	ProcessedInfo processor(const QString url);
	ProcessedInfo   AsyncProcessor::handleReply(const QString& reply, const QString& currentUrl);

private:
	ScanningConfiguration &mConfig;
	std::set<std::string> &mUniqueURLs;
	std::deque<std::string> &mDeque;
	QList<QPair<QString, URLStatus>> &mList;
	std::mutex mMutex;
};

#endif