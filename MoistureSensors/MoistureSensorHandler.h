#ifndef MOISTURE_SENSOR_HANDLER_H
#define MOISTURE_SENSOR_HANDLER_H

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

#include <AnalogSensorHandler.h>


using namespace SF;

//---------------------------------------------------

class MoistureSensorHandler : public AnalogSensorHandler{
public:
	MoistureSensorHandler(	std::shared_ptr<ScheduleHandler> scheduleHndlr,

							std::shared_ptr<ESP8266WebServer> server,
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

							int MAX_WAIT_PERIOD_ON_DRY_SOIL = 5 * 60 * 60);

private:
	std::shared_ptr<ScheduleHandler> scheduleHndlr;

public:
	void setServerURLs();

	void update();

	//------------------

	void requestDailySensorDataFromServerIfNotAlreadyDone();
	bool successfullyReceivedSensorDataToday() const;

	//------------------
	
	bool checkIfSoilIsDry();

	bool shallDrySoilBeIrrigated();

	bool shallIrrigateDespiteIrrigationPlan();

	void addDroughtIrrigationEntries();

	void checkForIrrigationDueToDryness();

	bool temperatureOkForIrrigation();
};

#endif
