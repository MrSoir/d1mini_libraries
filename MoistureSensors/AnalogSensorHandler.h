#ifndef ANALOG_SENSOR_HANDLER_H
#define ANALOG_SENSOR_HANDLER_H

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include <string>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <utility>
#include <map>
#include <set>
#include <memory>

#include <StaticFunctions.h>

#include <Task.h>
#include <OneTimerTask.h>
#include <RecurringTask.h>
#include <ScheduleBD.h>
#include <Scheduler.h>
#include <ScheduleHandler.h>

#include <AnalogSensorPinEntry.h>

#include <multiplexer.h>

#include <ServerDataUpdater.h>


using namespace SF;

//---------------------------------------------------

class AnalogSensorHandler: public ServerDataUpdater{
public:
	AnalogSensorHandler(std::shared_ptr<ESP8266WebServer> server,
						std::shared_ptr<String> SERVER_MDNS,
						std::shared_ptr<String> SERVER_PORT,

						std::shared_ptr<String> ARDUINO_ID,

						String SERVER_REQUEST_BASE_PATH,

						std::shared_ptr<unsigned long> UNIX_TIME, 
						std::shared_ptr<int> UNIX_DAY_OFFSET,
						
						std::vector<int> MULTIPLEXER_CONTROL_PINS,
						std::vector<int> MULTIPLEXER_ANALOG_INPUT_PINS,
						int MULTIPLEXER_ANALOG_OUTPUT_PIN = A0,
						
						float MIN_MOISTURE_SENSOR_VALUE = 200.0,
						float MAX_MOISTURE_SENSOR_VALUE = 1000.0,

						int MAX_WAIT_PERIOD_ON_DRY_SOIL = 2 * 60 * 60);


protected:
	std::vector<AnalogSensorPinEntry> ANALOG_SEN_ENTRIES;
	
	Multiplexer multiplexer;

	int MAX_WAIT_PERIOD_ON_DRY_SOIL = 2 * 60 * 60;
	
	float MIN_MOISTURE_SENSOR_VALUE = 200.0;
	float MAX_MOISTURE_SENSOR_VALUE = 1000.0;

	//-----

	std::shared_ptr<ESP8266WebServer> server;
	std::shared_ptr<String> SERVER_MDNS;
	std::shared_ptr<String> SERVER_PORT;

	std::shared_ptr<String> ARDUINO_ID;

	String SERVER_REQUEST_BASE_PATH;

	std::shared_ptr<unsigned long> UNIX_TIME;
	std::shared_ptr<int> UNIX_DAY_OFFSET;

	//-----

	// ServerDataUpdater-derived function:
	virtual void consumeDailyServerData(DynamicJsonDocument& JSONdoc) override;

	void receiveAndConsumeAnalogSensorEntries();

public:
	void setServerURLs();

	void receiveAndConsumeAnalogSensorEntry();

	void removeAnalogSensorEntry();

	void clearAnalogSensorEntries();


protected:
	void receiveAndConsumeAnalogSensorMinMaxValues();
	void replaceAnalogMinMaxValues();

	String genAnalogSensorJSONstr_hlpr(SensorType tarType, 
									   void (*JSONGenerator)(const AnalogSensorPinEntry& entry, JsonArray JSONsensors));

	String genAnalogSensorEntriesJSONstr(SensorType tarType = SensorType::ANY);

	String genAnalogSensorValuesJSONstr(SensorType tarType = SensorType::ANY);

	void sendAnalogSensorEntries();
	void sendAnalogSensorValues();

	void replaceAnalogSensorEntries(const JsonArray& sensors);

	void readAnalogPins();
	unsigned long lastTimeValuesRead = 0;

	void addAnalogSensorEntry( String id,
				               int pin,
				               float sensitivity,
				               SensorType type);
};

#endif
