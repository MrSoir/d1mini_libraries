#ifndef SCHEDULE_BD_H
#define SCHEDULE_BD_H

#include <Arduino.h>

#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <utility>
#include <map>
#include <set>

#include <StaticFunctions.h>
#include <Task.h>
#include <Scheduler.h>

class Scheduler;

class ScheduleBD
{
public:
	ScheduleBD();
	
	void add(Task* ie);
	
	void remove(Task* ie);
	
	void clear();
	
	int indexOf(const Task* ie) const;
	
	bool contains(const Task* ie) const;

	const std::vector<Task*>& entries() const;
	
	void startEntries(Scheduler& scheduler, unsigned long UNIX_TIME, int UNIX_DAY_OFFSET);
	void removeExpiredEntries(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET);
	
	void resetExecutionStarts();
	
	void startTask(Task* ie, Scheduler& scheduler, unsigned long UNIX_TIME, int UNIX_DAY_OFFSET);
	
	unsigned long timeTilNextExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const;

	String createArduinoStringFromAllEntries() const;

	void printEntries() const;
private:
	std::vector<Task*> _entries;
};


#endif
