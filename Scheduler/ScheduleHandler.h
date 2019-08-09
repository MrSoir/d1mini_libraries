#ifndef SCHEDULE_HANDLER_H
#define SCHEDULE_HANDLER_H

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
#include <Task.h>
#include <OneTimerTask.h>
#include <RecurringTask.h>
#include <ScheduleBD.h>
#include <Scheduler.h>

using namespace SF;

class ScheduleHandler
{
public:
	ScheduleHandler(std::shared_ptr<ESP8266WebServer> server,
					  std::shared_ptr<String> SERVER_MDNS,
					  std::shared_ptr<String> SERVER_PORT,

					  std::shared_ptr<String> ARDUINO_ID,

					  std::shared_ptr<unsigned long> UNIX_TIME, 
					  std::shared_ptr<int> UNIX_DAY_OFFSET, 
					  int EXECUTION_PIN,
					  String SERVER_REQUEST_BASE_PATH);
	
	void update();

	void requestDailyScheduleFromServerIfNotAlreadyDone();

	bool successfullyReceivedScheduleToday() const;

	void addTask(Task* ie);
	void removeTask(Task* ie);
	void clearSchedule();

	ScheduleBD schedule;
	Scheduler scheduler;

private:
	void setServerURLs();

//------------------manual irrigation----------------------------

	void receiveManualExecutionRequest();
	void startManualExecution(int duration);
	void receiveStopRequest();
	void stopCurrentExecution();

	//------------------CRUD irrigation plan----------------------------

	Task* createTask(unsigned long begin, int duration, int weekdays=-1);
	
	Task* evalTaskFromRequest();
	
	void receiveAndAddTask();
	void receiveAndRemoveTask();
	
	void receiveSchedule();
	
	void consumeClearScheduleRequest();

	void sendSchedule();

	void receiveScheduleFromServer();

	void replaceTasks(const JsonArray& itms);

//---------------

	std::shared_ptr<ESP8266WebServer> server;
	std::shared_ptr<String> SERVER_MDNS;
	std::shared_ptr<String> SERVER_PORT;
	std::shared_ptr<String> ARDUINO_ID;

	std::shared_ptr<unsigned long> UNIX_TIME;
	std::shared_ptr<int> UNIX_DAY_OFFSET;
	int EXECUTION_PIN;

	bool SUCC_RCVD_IRGN_PLN_TODAY;
	unsigned long LST_SRVR_DATA_UPDTD;

	String SERVER_REQUEST_BASE_PATH;
};


#endif

