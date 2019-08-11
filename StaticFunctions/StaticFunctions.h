#ifndef STATIC_FUNCTIONS_H
#define STATIC_FUNCTIONS_H

#include <Arduino.h>
#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <utility>
#include <map>
#include <set>
#include <stdlib.h>     /* strtoul */


namespace SF
{
	std::vector<String> splitString(String str, char sep);

	int evalDayOfWeek(unsigned long unix_time, int UNIX_DAY_OFFSET);
	int evalCurrentDayOfWeek(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET);

	int evalDayTime(unsigned long unix_time);
	int evalCurrentDayTime(unsigned long UNIX_TIME);

	int evalDayEndTime(int startDayTime, int duration);
	int powInt(int base, int exponent);

	std::vector<bool> daysOfWeekIntToBoolArr(int powerVal);

	int arrayToDaysWofWeek(JsonArray arr);

	std::tuple<DynamicJsonDocument, int> serverGET(String url, const size_t bufferSize = 100, const uint16_t timeout = 10000);
	String genServerRequestUrl(String urlRelPath, const String& SERVER_MDNS, const String& SERVER_PORT, const String& ARDUINO_ID);

	unsigned long hexStringToULong(const std::string& hex_str);
	std::tuple<unsigned long,unsigned long,unsigned long> hexColorStringToULongs(const std::string& hexColStr);
	std::tuple<unsigned long,unsigned long,unsigned long> hexColorStringToULongs(const String& hexColStr);
}

#endif



