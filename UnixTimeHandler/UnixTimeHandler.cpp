#include "UnixTimeHandler.h"



//----------------------public functions-----------------------------



UnixTimeHandler::UnixTimeHandler(std::shared_ptr<ESP8266WebServer> server,
								 std::shared_ptr<String> SERVER_MDNS,
								 std::shared_ptr<String> SERVER_PORT,
								 
								 std::shared_ptr<String> ARDUINO_ID,
								 
								 std::shared_ptr<unsigned long> UNIX_TIME, 
								 std::shared_ptr<int> UNIX_DAY_OFFSET)
	: 	SERVER_MDNS(SERVER_MDNS),
		server(server),
		ARDUINO_ID(ARDUINO_ID),
		SERVER_PORT(SERVER_PORT),
		UNIX_TIME(UNIX_TIME),
		UNIX_DAY_OFFSET(UNIX_DAY_OFFSET),
		lastMillisCheck(0),
		UNIX_TIME_MILLIS(0),
		SUCC_RCVD_UNIX_TME_TODAY(false),
		LST_UNIX_TIME_UPDTD(0)
{
	setServerURLs();
}

void UnixTimeHandler::requestDailyTimeFromServerIfNotAlreadyDone()
{
	// once a day or on boot, request server data: 
	if(*UNIX_TIME < 86400 || *UNIX_TIME / 86400 != LST_UNIX_TIME_UPDTD)
	{
		Serial.println("\nupdateServerData: -> new day!\n");

		SUCC_RCVD_UNIX_TME_TODAY = false;

		LST_UNIX_TIME_UPDTD = *UNIX_TIME / 86400;
		
		Serial.println("receiveUnixTimeFromServer:");
		receiveUnixTimeFromServer();
		Serial.println();
	}
	// server-requests fail quite often - try several times a day until request succeeds:
	if( !SUCC_RCVD_UNIX_TME_TODAY )
	{
		for(int i=0; i < 10; ++i){
			if( SUCC_RCVD_UNIX_TME_TODAY ){
				break;
			}
			if( !SUCC_RCVD_UNIX_TME_TODAY )
			{
				Serial.println("receiveUnixTimeFromServer:");
				receiveUnixTimeFromServer();
				Serial.println();
			}
		}
	}
}

bool UnixTimeHandler::successfullyReceivedUnixtTimeToday() const
{
	return SUCC_RCVD_UNIX_TME_TODAY;
}

void UnixTimeHandler::updateUnixTime()
{
	auto currentMillis = millis();

	// dt in milliseconds
	unsigned long dt = 0;
	if(currentMillis < lastMillisCheck)
	{
		// overflow: millis() retursn millis sinces d1-mini-boot -> millis() returns 'unsigned long' (4 bytes) -> max value == 2^32 - 1 == 4,294,967,295 -> 4,294,967,295 / (86400 * 1000) ~ 50 Tage
		//		-> overflow after ~50 days
		
		unsigned long maxMillis = 0;
		maxMillis -= 1; // now maxMillis is maxvalue (forced overflow)
		
		unsigned long dt0 = maxMillis - lastMillisCheck + 1; // dt0: dt between t=lastMillisCheck until maxMillis (overflow)
		unsigned long dt1 = currentMillis;									 // dt1: dt between t=0 (overflow) and currentMillis
		dt = dt0 + dt1;
	}else{
		dt = currentMillis - lastMillisCheck;
	}
	
	*UNIX_TIME += (UNIX_TIME_MILLIS + dt) / 1000; // dt in milliseconds <=> UNIX_TIME == seconds since epoch
	UNIX_TIME_MILLIS = (UNIX_TIME_MILLIS + dt) % 1000;

	lastMillisCheck = currentMillis;
}



//----------------------private functions-----------------------------



void UnixTimeHandler::setServerURLs()
{
	server->on("/setUnixTime",			[&](){receiveUnixTime();});
	server->on("/getUnixTime",			[&](){UnixTimeHandler::sendUnixTime();});

	server->on("/setUnixDayOffset",		[&](){receiveAndSetUnixDayOffset();});

	// example usage in browser:
//	
//	http://192.168.2.110/getUnixTime
//	http://192.168.2.110/setUnixTime?UNIX=999

//	http://192.168.2.110/setUnixDayOffset?offset=3
}


void UnixTimeHandler::receiveUnixTimeFromServer()
{
	String unix_time_server_url = SF::genServerRequestUrl("/irrigation/getUnixTime", *SERVER_MDNS, *SERVER_PORT, *ARDUINO_ID);
	
	std::tuple<DynamicJsonDocument, int> resltTpl = SF::serverGET(unix_time_server_url);
	auto loadedSuccessfully = std::get<1>(resltTpl);
	if(loadedSuccessfully)
	{
		auto jsonDoc = std::get<0>(resltTpl);

		SUCC_RCVD_UNIX_TME_TODAY = true;
		
		unsigned long serverUnixTime = jsonDoc["UNIX"];

		*UNIX_TIME = serverUnixTime;
		
		Serial.print("UNIX from server:");
		Serial.println(serverUnixTime);
	}else{
		Serial.print("ERROR receiveUnixTimeFromServer - loadedSuccessfully: ");
		Serial.println(loadedSuccessfully);
	}
}

void UnixTimeHandler::receiveUnixTime()
{
	int unixTime = server->arg("unix").toInt();
	if(unixTime == 0)
	{
		unixTime = server->arg("UNIX").toInt();
	}
	if(unixTime == 0)
	{
		String returnstr("invalid UNIX time received!");
		server->send(200, "text/plain", returnstr);
	}else{
		setDateTime(unixTime);
	
		String returnstr("UNIX_TIME set: ");
		returnstr.concat(*UNIX_TIME);
		server->send(200, "text/plain", returnstr);
	}
}
void UnixTimeHandler::sendUnixTime()
{
	String returnstr("UNIX_TIME: ");
	returnstr.concat(*UNIX_TIME);
	server->send(200, "text/plain", returnstr);
	Serial.println("sending UnixTime...");
}
void UnixTimeHandler::setDateTime(int unixTime)
{
	*UNIX_TIME = unixTime;
}

void UnixTimeHandler::receiveAndSetUnixDayOffset()
{
	int dayOffs = server->arg("offset").toInt();
	if(dayOffs >= 0 && dayOffs < 7)
	{
		Serial.print("UNIX_DAY_OFFSET received: ");
		Serial.println(dayOffs);
		setUnixDayOffset(dayOffs);
		String rets = "UNIX_DAY_OFFSET set: ";
		rets.concat(dayOffs);
		server->send(200, "text/plain", rets);
	}else{
		Serial.println("invalid UNIX_DAY_OFFSET received!");
		server->send(200, "text/plain", "invalid UNIX_DAY_OFFSET received!");
	}
}
void UnixTimeHandler::setUnixDayOffset(int dayOffs)
{
	*UNIX_DAY_OFFSET = dayOffs;
}

