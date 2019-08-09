#ifndef ONE_TIMER_TASK_H
#define ONE_TIMER_TASK_H

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

class OneTimerTask : public Task{
public:
	unsigned long begin = 0;
	int duration		= 0;
	
	OneTimerTask(unsigned long begin=0,
			     int duration=0);

	virtual int getType() const override;
	
	virtual bool operator==(const Task& ie) const override;

	virtual bool shouldStart(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const override;

	virtual unsigned long getStartTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const override;
	virtual unsigned long getEndTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const override;

	virtual unsigned long timeTilExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const override;

	virtual String toArduinoString() const override;

	virtual void print() const override;
	virtual void println() const override;
};


#endif
