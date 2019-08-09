#ifndef UNIX_TIME_HANDLER_H
#define UNIX_TIME_HANDLER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>

#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <memory>

#include <StaticFunctions.h>

using namespace SF;

class UnixTimeHandler
{
public:
	UnixTimeHandler(std::shared_ptr<ESP8266WebServer> server,
				    std::shared_ptr<String> SERVER_MDNS,
				    std::shared_ptr<String> SERVER_PORT,
					
				    std::shared_ptr<String> ARDUINO_ID,
					
					std::shared_ptr<unsigned long> UNIX_TIME, 
					std::shared_ptr<int> UNIX_DAY_OFFSET);
	
	void updateUnixTime();

	void requestDailyTimeFromServerIfNotAlreadyDone();

	bool successfullyReceivedUnixtTimeToday() const;

private:
	void receiveUnixTimeFromServer();

	void setServerURLs();

	void receiveUnixTime();
	void sendUnixTime();
	void setDateTime(int unixTime);
	void receiveAndSetUnixDayOffset();
	void setUnixDayOffset(int dayOffs);

//---------------
	
	std::shared_ptr<ESP8266WebServer> server;
	std::shared_ptr<String> SERVER_MDNS;
	std::shared_ptr<String> SERVER_PORT;
	std::shared_ptr<String> ARDUINO_ID;

	std::shared_ptr<unsigned long> UNIX_TIME;
	std::shared_ptr<int> UNIX_DAY_OFFSET;
	int IRRIGATION_PUMP_PIN;

	unsigned long lastMillisCheck;
	int UNIX_TIME_MILLIS;

	bool SUCC_RCVD_UNIX_TME_TODAY;
	unsigned long LST_UNIX_TIME_UPDTD;
};


#endif

