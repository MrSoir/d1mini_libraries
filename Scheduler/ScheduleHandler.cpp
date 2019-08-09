#include "ScheduleHandler.h"



//----------------------public functions-----------------------------



ScheduleHandler::ScheduleHandler(std::shared_ptr<ESP8266WebServer> server,
									 std::shared_ptr<String> SERVER_MDNS,
									 std::shared_ptr<String> SERVER_PORT,
									 
									 std::shared_ptr<String> ARDUINO_ID,
									 
									 std::shared_ptr<unsigned long> UNIX_TIME,
									 std::shared_ptr<int> UNIX_DAY_OFFSET,
									 int EXECUTION_PIN,
									 String SERVER_REQUEST_BASE_PATH)
	: 	SERVER_MDNS(SERVER_MDNS),
		server(server),
		ARDUINO_ID(ARDUINO_ID),
		SERVER_PORT(SERVER_PORT),
		UNIX_TIME(UNIX_TIME),
		UNIX_DAY_OFFSET(UNIX_DAY_OFFSET),
		EXECUTION_PIN(EXECUTION_PIN),
		SUCC_RCVD_IRGN_PLN_TODAY(false),
		LST_SRVR_DATA_UPDTD(0),
		schedule(ScheduleBD()),
		scheduler(Scheduler()),
		SERVER_REQUEST_BASE_PATH(SERVER_REQUEST_BASE_PATH)
{
	pinMode(EXECUTION_PIN, OUTPUT);
	
	setServerURLs();
}

void ScheduleHandler::update()
{
	// checks if currently one of the IrrigationEntries should schedule the irrigation:
	scheduler.update(schedule, *UNIX_TIME, *UNIX_DAY_OFFSET);
	
	scheduler.execute(*UNIX_TIME, EXECUTION_PIN);
}

void ScheduleHandler::requestDailyScheduleFromServerIfNotAlreadyDone()
{
	// once a day or on boot, request server data: 
	if(*UNIX_TIME < 86400 || *UNIX_TIME / 86400 != LST_SRVR_DATA_UPDTD)
	{
		Serial.println("\nupdateServerData: -> new day!\n");

		SUCC_RCVD_IRGN_PLN_TODAY = false;

		LST_SRVR_DATA_UPDTD = *UNIX_TIME / 86400;
	}

	if( !SUCC_RCVD_IRGN_PLN_TODAY )
	{
		Serial.println("receiveIrrigationPlanFromServer:");
		receiveScheduleFromServer();
		Serial.println();
	}
}

bool ScheduleHandler::successfullyReceivedScheduleToday() const
{
	return SUCC_RCVD_IRGN_PLN_TODAY;
}



//----------------------private functions-----------------------------



void ScheduleHandler::setServerURLs()
{
	server->on("/addIrrigationEntry",		[&](){receiveAndAddTask();});
	server->on("/removeIrrigationEntry", 	[&](){receiveAndRemoveTask();});
	server->on("/setIrrigationPlan",		[&](){receiveSchedule();});
	server->on("/clearIrrigationPlan",	 	[&](){consumeClearScheduleRequest();});
	server->on("/getIrrigationPlan",		[&](){sendSchedule();});

	server->on("/startManualIrrigation", 	[&](){receiveManualExecutionRequest();});
	server->on("/stopCurrentIrrigation", 	[&](){receiveStopRequest();});

	// example usage in browser:
//	http://192.168.2.110/addIrrigationEntry?weekdays=1000&begin=99&duration=18
//	http://192.168.2.110/removeIrrigationEntry?weekdays=1000&begin=99&duration=18
//	http://192.168.2.110/clearIrrigationPlan
//	
//	http://192.168.2.110/sendIrrigationPlan?weekdays=127-127&begin=15-20&duration=5-10
//	http://192.168.2.110/getIrrigationPlan
//	
//	
//	http://192.168.2.110/startManualIrrigation?duration=5
//	http://192.168.2.110/stopCurrentIrrigation
}


void ScheduleHandler::receiveManualExecutionRequest()
{
	int duration = server->arg("duration").toInt();
	startManualExecution(duration);

	String returnstr("started irrigation for ");
	returnstr.concat(duration);
	returnstr.concat(" seconds!");
	server->send(200, "text/plain", returnstr);
}
void ScheduleHandler::startManualExecution(int duration)
{
	int irrigEnd = (*UNIX_TIME) + duration;
	scheduler.start(irrigEnd);
}
void ScheduleHandler::receiveStopRequest()
{
	stopCurrentExecution();
	server->send(200, "text/plain", "irrigation stopped!");
}
void ScheduleHandler::stopCurrentExecution()
{
	scheduler.stop();
}

//------------------CRUD irrigation plan----------------------------

Task* ScheduleHandler::createTask(unsigned long begin, int duration, int weekdays)
{
	if(weekdays == -1)
	{
	return new RecurringTask(weekdays, begin, duration);
	}else{
	return new OneTimerTask(begin, duration);
	}
}

Task* ScheduleHandler::evalTaskFromRequest()
{
	int weekdays = -1;

	int type = server->arg("type").toInt();

	int begin = server->arg("begin").toInt();
	int duration = server->arg("duration").toInt();
	
	if(type == 0)
	{
	int weekdays = server->arg("weekdays").toInt();
		return new RecurringTask(weekdays, begin, duration);
	}else if(type == 1){
		return new OneTimerTask(begin, duration);
	}else{
		Serial.print("unknown Task type: ");
		Serial.println(type);
		return nullptr;
	}
}

void ScheduleHandler::receiveAndAddTask()
{
	auto* ie = evalTaskFromRequest();

	if(ie)
	{
		addTask(ie);
	}else{
		Serial.println("receiveAndAddTask: ie == nullptr!");
	}

	server->send(200, "text/plain", "irrigation entry received and added!");
}
void ScheduleHandler::receiveAndRemoveTask()
{
	auto* ie = evalTaskFromRequest();

	if(ie)
	{
		removeTask(ie);
	}else{
		Serial.println("receiveAndRemoveTask: ie == nullptr!");
	}
	
	server->send(200, "text/plain", "irrigation entry received and removed!");
}

void ScheduleHandler::receiveSchedule()
{
	String plan = server->arg("entries");
	
	auto entries = splitString(plan, '|');

	clearSchedule();
	
	for(auto entry: entries)
	{
	auto vals = splitString(entry, '-');
	auto entryId = vals[0];

	auto begin	= vals[1].toInt();
	auto duration = vals[2].toInt();
	
	if(entryId == "0")
	{
		auto weekdays = vals[3].toInt();
		schedule.add( new RecurringTask(weekdays, begin, duration) );
		
	}else if(entryId == "1")
	{
		schedule.add( new OneTimerTask(begin, duration) );
	}else{
		String msg("invalid plan received - unknown entry-id: ");
		msg.concat(entryId);
		server->send(4000, "text/plain", msg);
	}
	}

	Serial.println("\nreceiveIrrigationPlan:");
	schedule.printEntries();
	Serial.println();

	// as response to request to signal plan was received correctly:
	sendSchedule();
}

void ScheduleHandler::addTask(Task* ie)
{
	schedule.add(ie);
	schedule.printEntries();
}
void ScheduleHandler::removeTask(Task* ie)
{
	schedule.remove(ie);
	schedule.printEntries();
}

void ScheduleHandler::consumeClearScheduleRequest()
{
	clearSchedule();
	server->send(200, "text/plain", "IrrigationPlan cleared!");
}
void ScheduleHandler::clearSchedule()
{
	schedule.clear();
}

void ScheduleHandler::sendSchedule()
{
	auto plan_str = schedule.createArduinoStringFromAllEntries();
	
	server->send(200, "text/plain", plan_str);
	
	Serial.print("sendIrrigationPlan: ");
	Serial.println(plan_str);
}

void ScheduleHandler::receiveScheduleFromServer()
{
	String serverReqBsePth = SERVER_REQUEST_BASE_PATH + "/getIrrigationPlan";
	String irgn_pln_server_url = SF::genServerRequestUrl(serverReqBsePth, *SERVER_MDNS, *SERVER_PORT, *ARDUINO_ID);
	
	std::tuple<DynamicJsonDocument, int> resltTpl = SF::serverGET(irgn_pln_server_url, 10000);
	
	auto loadedSuccessfully = std::get<1>(resltTpl);
	if(loadedSuccessfully)
	{
		auto jsonDoc = std::get<0>(resltTpl);
		JsonObject root = jsonDoc.as<JsonObject>();

		// the easy way:
	//	JsonArray itms = jsonDoc["irrigationEntries"];
		
		// the hard way:
		// (is more generic and doesn't rely on the keyword "irrigationEntries" -> so whatever the server will call this data ("irrigationEntries"/"arduinoIrrigationEntries"/"serverIrrigationEntries"/"data") -> the Arduino can handle it due to generic approach!)
		auto bgnIt = root.begin();
		if(bgnIt != root.end())
		{
			JsonArray itms = bgnIt->value().as<JsonArray>();

			replaceTasks(itms);
		}else{
			Serial.println("receiveIrrigationPlanFromServer -> root.begin == root.end -> json-object received from server contains no data!!!");
		}
	}else{
		Serial.print("ERROR receiveIrrigationPlanFromServer - loadedSuccessfully: ");
		Serial.println(loadedSuccessfully);
	}
}
void ScheduleHandler::replaceTasks(const JsonArray& itms)
{
	Serial.print("replaceIrrigationEntries - irrigationEntries: ");
	serializeJson(itms, Serial);
	Serial.println();

	SUCC_RCVD_IRGN_PLN_TODAY = true;

	clearSchedule();

	Serial.print("entries.size: ");
	Serial.println(itms.size());
	
	for(size_t i=0; i < itms.size(); ++i)
	{
		auto itm = itms[i];

		auto ieType = itm["type"];

		if(ieType == 0)
		{
			JsonArray dowarr = itm["daysOfWeek"];
			int begin	= itm["begin"];
			int duration = itm["duration"];

			schedule.add( new RecurringTask(dowarr, begin, duration) );
		
		}else if(ieType == 1)
		{
			int begin	= itm["begin"];
			int duration = itm["duration"];

			schedule.add( new OneTimerTask(begin, duration) );
			
		}else{
			String msg("invalid entry-type: ");
			msg.concat(String(static_cast<int>(itm["type"])));
			Serial.println(msg);
		}
	}
	
	Serial.println("\nreplaceIrrigationEntries:");
	schedule.printEntries();
	Serial.println();
}
