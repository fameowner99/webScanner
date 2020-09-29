#ifndef ScanningConfiguration_h

#define ScanningConfiguration_h

struct ScanningConfiguration
{
	QString url;
	QString textToFind;
	size_t	maxNumberOfThreads = 0;
	size_t	maxNumberOfScanningURLs = 0;
};


#endif