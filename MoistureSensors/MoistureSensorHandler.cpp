#include "MoistureSensorHandler.h"

MoistureSensorHandler::MoistureSensorHandler(std::shared_ptr<ScheduleHandler> scheduleHndlr,
											 
											 std::shared_ptr<ESP8266WebServer> server,
											 std::shared_ptr<String> SERVER_MDNS,
											 std::shared_ptr<String> SERVER_PORT,

											 std::shared_ptr<String> ARDUINO_ID,

											 String SERVER_REQUEST_BASE_PATH,

											 std::shared_ptr<unsigned long> UNIX_TIME, 
											 std::shared_ptr<int> UNIX_DAY_OFFSET,
											
											 std::vector<int> MULTIPLEXER_CONTROL_PINS,
											 std::vector<int> MULTIPLEXER_ANALOG_INPUT_PINS,
											 int MULTIPLEXER_ANALOG_OUTPUT_PIN,
											
											 float MIN_MOISTURE_SENSOR_VALUE,
											 float MAX_MOISTURE_SENSOR_VALUE,

											 int MAX_WAIT_PERIOD_ON_DRY_SOIL)
	:	AnalogSensorHandler(server,
							SERVER_MDNS,
							SERVER_PORT,

							ARDUINO_ID,

							SERVER_REQUEST_BASE_PATH,

							UNIX_TIME, 
							UNIX_DAY_OFFSET,
							
							MULTIPLEXER_CONTROL_PINS,
							MULTIPLEXER_ANALOG_INPUT_PINS,
							MULTIPLEXER_ANALOG_OUTPUT_PIN,
							
							MIN_MOISTURE_SENSOR_VALUE,
							MAX_MOISTURE_SENSOR_VALUE,

							MAX_WAIT_PERIOD_ON_DRY_SOIL),
		scheduleHndlr(scheduleHndlr)
	{
		setServerURLs();
	}

//---------------------------------------------------------

void MoistureSensorHandler::setServerURLs()
{
	server->on("/getMoistureSensorEntries",		[&](){sendMoistureSensorEntries();});
	server->on("/getMoistureSensorValues",		[&](){sendMoistureSensorValues();});
	server->on("/setMoistureSensorEntries",		[&](){receiveAndConsumeAnalogSensorEntries();});
}

void MoistureSensorHandler::update()
{
	checkForIrrigationDueToDryness();
}

//---------------------------------------------------------
String MoistureSensorHandler::genMoisturesSensorEntriesJSONstr()
{
	return genAnalogSensorEntriesJSONstr(SensorType::MOISTURE_SENSOR);
}
String MoistureSensorHandler::genMoisturesSensorValuesJSONstr()
{
	return genAnalogSensorValuesJSONstr(SensorType::MOISTURE_SENSOR);
}


void MoistureSensorHandler::sendMoistureSensorEntries(){
	String JSONstr = genMoisturesSensorEntriesJSONstr();
	
	server->send(200, "text/plain", JSONstr);
}
void MoistureSensorHandler::sendMoistureSensorValues(){
	String JSONstr = genMoisturesSensorValuesJSONstr();
	
	server->send(200, "text/plain", JSONstr);
}

//---------------------------------------------------------

bool MoistureSensorHandler::checkIfSoilIsDry()
{
	// alle 10 Minuten checken reicht vollkommen aus:
	if((*UNIX_TIME - lastTimeDrySoilChecked) < CHECK_FOR_DROUGHT_INTERVAL){
		return false;
	}

	lastTimeDrySoilChecked = *UNIX_TIME; 

	readAnalogPins();

	int i=0;
	for(const auto& entry: ANALOG_SEN_ENTRIES)
	{
		if(entry.type == SensorType::MOISTURE_SENSOR)
		{
			Serial.print("moistureSensor[");
			Serial.print(i++);
			Serial.print("].absValue: ");
			Serial.print(entry.absValue());
			Serial.print("	relValue: ");
			Serial.println(entry.relValue());
			Serial.print("	sensitivity: ");
			Serial.println(entry.sensitivity);
			if(entry.shallTrigger())
			{
				Serial.println("sensor is triggering!");
				return true;
			}
		}
	}
	//Serial.println();
	return false;
}

bool MoistureSensorHandler::shallDrySoilBeIrrigated()
{
	auto dt = scheduleHndlr->schedule.timeTilNextExecution(*UNIX_TIME, *UNIX_DAY_OFFSET);
	return dt > MAX_WAIT_PERIOD_ON_DRY_SOIL;
}

bool MoistureSensorHandler::shallIrrigateDespiteIrrigationPlan()
{
	return !scheduleHndlr->scheduler.isRunning(*UNIX_TIME) && checkIfSoilIsDry() && shallDrySoilBeIrrigated() && temperatureOkForIrrigation();
}

void MoistureSensorHandler::addDroughtIrrigationEntries()
{
	auto begin = *UNIX_TIME;
	for(int i=0; i < 3; ++i)
	{
		auto dur = 60 * 2;
		Serial.print("adding drought-oneTimer:	begin: ");
		Serial.print(begin);
		Serial.print("	duration: ");
		Serial.println(dur);
		auto* oneTimer = new OneTimerTask(begin, dur);
		scheduleHndlr->schedule.add(oneTimer);

		begin += 5 * 60;
	}
	Serial.println("	-> addDroughtIrrigationEntries - updated plan:");
	scheduleHndlr->schedule.printEntries();
}

void MoistureSensorHandler::checkForIrrigationDueToDryness()
{
	if( shallIrrigateDespiteIrrigationPlan() )
	{
		addDroughtIrrigationEntries();
	}
}

bool MoistureSensorHandler::temperatureOkForIrrigation()
{
	return true;
}
