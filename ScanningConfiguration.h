#ifndef ScanningConfiguration_h

#define ScanningConfiguration_h

struct ScanningConfiguration
{
	QString url;
	QString textToFind;
	size_t	maxNumberOfThreads;
	size_t	maxNumberOfScanningURLs;
};


#endif