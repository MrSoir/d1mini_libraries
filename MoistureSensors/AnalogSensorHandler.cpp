#include "AnalogSensorHandler.h"


AnalogSensorHandler::AnalogSensorHandler(std::shared_ptr<ESP8266WebServer> server,
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
	: 	ServerDataUpdater(server,
							SERVER_MDNS,
							SERVER_PORT,

							ARDUINO_ID,

							UNIX_TIME, 
							UNIX_DAY_OFFSET, 

							SERVER_REQUEST_BASE_PATH),
		
		ANALOG_SEN_ENTRIES(std::vector<AnalogSensorPinEntry>()),
		
		multiplexer(MULTIPLEXER_CONTROL_PINS, MULTIPLEXER_ANALOG_INPUT_PINS, MULTIPLEXER_ANALOG_OUTPUT_PIN),
		
		MAX_WAIT_PERIOD_ON_DRY_SOIL(MAX_WAIT_PERIOD_ON_DRY_SOIL),
		
		MIN_MOISTURE_SENSOR_VALUE(MIN_MOISTURE_SENSOR_VALUE),
		MAX_MOISTURE_SENSOR_VALUE(MAX_MOISTURE_SENSOR_VALUE),

		server(server),
		SERVER_MDNS(SERVER_MDNS),
		SERVER_PORT(SERVER_PORT),
		ARDUINO_ID(ARDUINO_ID),
		SERVER_REQUEST_BASE_PATH(SERVER_REQUEST_BASE_PATH),
	
		UNIX_TIME(UNIX_TIME),
		UNIX_DAY_OFFSET(UNIX_DAY_OFFSET)
	{
		setServerURLs();
	}

//-----------------------------------------
// ServerDataUpdater-derived function:
void AnalogSensorHandler::consumeDailyServerData(DynamicJsonDocument& JSONdoc)
{
	JsonArray sensors = JSONdoc["sensors"];

	replaceAnalogSensorEntries(sensors);
}


void AnalogSensorHandler::setServerURLs()
{
	server->on("/addAnalogSensorEntry",			[&](){receiveAndConsumeAnalogSensorEntry();});
	server->on("/removeAnalogSensorEntry",		[&](){removeAnalogSensorEntry();});
	server->on("/getAnalogSensorEntries",		[&](){sendAnalogSensorEntries();});
	server->on("/getAnalogSensorValues",		[&](){sendAnalogSensorValues();});
	server->on("/clearAnalogSensorEntries",		[&](){clearAnalogSensorEntries();});
	
//	server->on("/setAnalogSensorMinMaxValues",	[&](){receiveAndConsumeAnalogSensorMinMaxValues();});
}

//-----------------------------------------

/*
void AnalogSensorHandler::receiveAndConsumeAnalogSensorMinMaxValues()
{
	auto potJSONdoc = SF::serverParseJSONbody(server);
	if(std::get<1>(potJSONdoc) == 1){

		auto JSONdoc = std::get<0>(potJSONdoc);

		int minVal = JSONdoc["min"];
		int maxVal = JSONdoc["max"];

		MIN_MOISTURE_SENSOR_VALUE = minVal;
		MAX_MOISTURE_SENSOR_VALUE = maxVal;

		for(auto& pe: ANALOG_SEN_ENTRIES){
			pe.setMinAbsValue(minVal);
			pe.setMaxAbsValue(maxVal);
		}

		String msg("successfully received Analog Sensor Min- & Max-Values!");
		server->send(200, "text/plain", msg);
	}
}
void AnalogSensorHandler::replaceAnalogMinMaxValues(const JsonArray& sensors)
{
	Serial.println("replaceAnalogSensorEntries - clearing old analog entries");
	clearAnalogSensorEntries();

	Serial.print("replaceAnalogSensorEntries:\nmoistureSensors.size: ");
	Serial.println(sensors.size());
	
	for(int i=0; i < sensors.size(); ++i)
	{
		auto snsr = sensors[i];
		String id					 = snsr["id"];
		int pin						 = snsr["pin"];
		float sensitivity	 		 = snsr["sensitivity"];

		int minVal 				 	 = snsr["min"];
		int maxVal 				 	 = snsr["max"];

		Serial.print("	moistSensor[");
		Serial.print(i);
		Serial.print("] -> id: ");
		Serial.print(id);
		Serial.print(", pin: ");
		Serial.print(pin);
		Serial.print(", sensitivity");
		Serial.println(sensitivity);

		auto ase = AnalogSensorPinEntry(id, pin, sensitivity);
		ase.setMinAbsValue(minVal);
		ase.setMaxAbsValue(maxVal);
		
		ANALOG_SEN_ENTRIES.push_back( ase );
		multiplexer.addSensor(pin);
	}
}*/

void AnalogSensorHandler::receiveAndConsumeAnalogSensorEntry()
{
	auto id = server->arg("id");
	auto pin = server->arg("pin").toInt();
	auto sensitivity = server->arg("sensitivity").toFloat();
	
	addAnalogSensorEntry(id, pin, sensitivity, SensorType::MOISTURE_SENSOR);
}

void AnalogSensorHandler::addAnalogSensorEntry( String id,
			               int pin,
			               float sensitivity,
			               SensorType type)
{
	ANALOG_SEN_ENTRIES.push_back( AnalogSensorPinEntry(	id, 
														pin, 
														sensitivity, 
														0, 
														MIN_MOISTURE_SENSOR_VALUE, 
														MAX_MOISTURE_SENSOR_VALUE,
														type) );
	multiplexer.addSensor(pin);
}

void AnalogSensorHandler::removeAnalogSensorEntry()
{
	auto id = server->arg("id");
	
	bool idExists = false;
	
	for(int i=0; i < ANALOG_SEN_ENTRIES.size(); ++i)
	{
		if(ANALOG_SEN_ENTRIES[i].id.equals(id) )
		{
			idExists = true;
			multiplexer.removeSensor( ANALOG_SEN_ENTRIES[i].pin );
			ANALOG_SEN_ENTRIES.erase( ANALOG_SEN_ENTRIES.begin() + i );
			
		}
	}
	if(idExists)
	{
		if(ANALOG_SEN_ENTRIES.size() > 0)
		{
			ANALOG_SEN_ENTRIES.shrink_to_fit();
		}
		server->send(200, "text/plain", "successfully deleted sensor-entry!");
	}else{
		String errorMsg("trying to delete AnalogSensor: ");
		errorMsg.concat(id);
		server->send(400, "text/plain", errorMsg);
	}
}


//------------------------

void generateJSONSensorEntry(const AnalogSensorPinEntry& entry, JsonArray JSONsensors)
{
	auto JSONsensor = JSONsensors.createNestedObject();
	JSONsensor["id"]					= String(entry.id);
	JSONsensor["value"]			 = String(entry.relValue());
	JSONsensor["absValue"]		= String(entry.absValue());
	JSONsensor["pin"]				 = String(entry.pin);
	JSONsensor["sensitivity"] = String(entry.sensitivity);
	JSONsensor["type"]				= String(entry.type);
}
void generateJSONSensorValue(const AnalogSensorPinEntry& entry, JsonArray JSONsensors)
{
	auto JSONsensor = JSONsensors.createNestedObject();
	JSONsensor["id"]					= String(entry.id);
	JSONsensor["value"]			 = String(entry.relValue());
}

String AnalogSensorHandler::genAnalogSensorJSONstr_hlpr(SensorType tarType, void (*JSONGenerator)(const AnalogSensorPinEntry& entry, JsonArray JSONsensors))
{
	readAnalogPins();

	StaticJsonDocument<10000> JSONdoc;

	auto JSONsensors = JSONdoc.createNestedArray("sensors");
	
	for(const auto& entry: ANALOG_SEN_ENTRIES)
	{
		if(tarType == SensorType::ANY ||
				tarType == entry.type)
		{
			JSONGenerator(entry, JSONsensors);
		}
	}
	
	String JSONstr;
	serializeJson(JSONdoc, JSONstr);
	return JSONstr;
}

String AnalogSensorHandler::genAnalogSensorEntriesJSONstr(SensorType tarType)
{
	return genAnalogSensorJSONstr_hlpr(tarType, generateJSONSensorEntry);
}

String AnalogSensorHandler::genAnalogSensorValuesJSONstr(SensorType tarType)
{
	return genAnalogSensorJSONstr_hlpr(tarType, generateJSONSensorValue);
}

void AnalogSensorHandler::sendAnalogSensorEntries()
{
	String JSONstr = genAnalogSensorEntriesJSONstr();
	
	server->send(200, "text/plain", JSONstr);
}
void AnalogSensorHandler::sendAnalogSensorValues()
{
	String JSONstr = genAnalogSensorValuesJSONstr();
	
	server->send(200, "text/plain", JSONstr);
}

//----------------

void AnalogSensorHandler::replaceAnalogSensorEntries(const JsonArray& sensors)
{
	Serial.println("replaceAnalogSensorEntries - clearing old analog entries");
	clearAnalogSensorEntries();

	Serial.print("replaceAnalogSensorEntries:\nmoistureSensors.size: ");
	Serial.println(sensors.size());
	
	for(int i=0; i < sensors.size(); ++i)
	{
		auto snsr = sensors[i];
		String id					 = snsr["id"];
		int pin						 = snsr["pin"];
		float sensitivity	 = snsr["sensitivity"];

		Serial.print("	moistSensor[");
		Serial.print(i);
		Serial.print("] -> id: ");
		Serial.print(id);
		Serial.print(", pin: ");
		Serial.print(pin);
		Serial.print(", sensitivity");
		Serial.println(sensitivity);

		addAnalogSensorEntry(id, pin, sensitivity, SensorType::MOISTURE_SENSOR);
	}
}

void AnalogSensorHandler::receiveAndConsumeAnalogSensorEntries()
{
	auto potJSONdoc = SF::serverParseJSONbody(server);
	if(std::get<1>(potJSONdoc) == 1){

		auto JSONdoc = std::get<0>(potJSONdoc);

		JsonArray sensors = JSONdoc["sensors"];

		replaceAnalogSensorEntries(sensors);

		String msg("successfully received Analog Sensor Entries!");
		server->send(200, "text/plain", msg);
	}
}

//----------------

void AnalogSensorHandler::clearAnalogSensorEntries()
{
	multiplexer.clearSensors();
	ANALOG_SEN_ENTRIES.clear();
	ANALOG_SEN_ENTRIES.shrink_to_fit();
}

//----------- eval sensor data:

void AnalogSensorHandler::readAnalogPins()
{
	multiplexer.readAnalogPins();
	
	for(int i=0; i < ANALOG_SEN_ENTRIES.size(); ++i)
	{
		int pin = ANALOG_SEN_ENTRIES[i].pin;
		auto sensorVal = multiplexer.getSensorValue(pin);

		ANALOG_SEN_ENTRIES[i].setAbsValue( sensorVal );

/*		Serial.print("Sensor[");
		Serial.print(pin);
		Serial.print("]: ");
		Serial.print(sensorVal);
		Serial.print("	|	abs: ");
		Serial.print(ANALOG_SEN_ENTRIES[i].absValue());
		Serial.print("	rel: ");
		Serial.println(ANALOG_SEN_ENTRIES[i].relValue());*/
	}
//	Serial.println();

	lastTimeValuesRead = *UNIX_TIME;
}
