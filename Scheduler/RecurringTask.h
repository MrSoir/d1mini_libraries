#ifndef RecurringIE_H
#define RecurringIE_H

#include <Arduino.h>
#include <ArduinoJson.h>

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

class RecurringTask : public Task
{
public:
	int weekdays	= 0;
	int begin		= 0;
	int duration	= 0;
	
	RecurringTask(int weekdays=0,
				int begin=0,
				int duration=0);
	RecurringTask(JsonArray weekdays,
				int begin=0,
				int duration=0);

	virtual int getType() const override;
	
	virtual bool operator==(const Task& ie) const override;

	virtual bool shouldStart(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const override;
	unsigned long getNextStartTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const;
	unsigned long getNextEndTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const;
	
	unsigned long getStartTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const override;
	unsigned long getEndTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const override;

	unsigned long timeTilNextExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const;
	virtual unsigned long timeTilExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const override;

	virtual String toArduinoString() const override;

	virtual void print() const override;
	virtual void println() const override;

private:
	bool executesThisCurrentDay(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const;
	bool alreadyExecutedToday(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const;
};


#endif


