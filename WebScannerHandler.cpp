#include "WebScannerHandler.h"
#include <iostream>

WebScannerHandler::WebScannerHandler()
{

}

WebScannerHandler::~WebScannerHandler() {}

void WebScannerHandler::setConfigurations(const ScanningConfiguration& aConfig)
{
	mConfig = aConfig;
}

void WebScannerHandler::startScanning()
{
	std::cout << "START" << std::endl;;


}

void WebScannerHandler::stopScanning()
{
	std::cout << "STOP" << std::endl;;
}

void WebScannerHandler::pauseScanning()
{
	std::cout << "PAUSE" << std::endl;;
}