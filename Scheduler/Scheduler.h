#ifndef Scheduler_H
#define Scheduler_H

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
#include <ScheduleBD.h>

class ScheduleBD;

class Scheduler
{
public:	
	void start(unsigned long execEnd);
	void stop();
	
	void execute(unsigned long UNIX_TIME, int EXECUTION_PIN);
	
	bool isRunning(unsigned long UNIX_TIME);
	unsigned long isRunningUntil() const;
	unsigned long isRunningFor(unsigned long UNIX_TIME) const;
	
	void update(ScheduleBD& schedule, unsigned long UNIX_TIME, int UNIX_DAY_OFFSET);
	
	void setExecutionEnd(unsigned long irrigEnd);

private:
	void resetExecutionStarts(ScheduleBD& schedule, unsigned long UNIX_TIME);
	bool alreadyResettedToday(unsigned long UNIX_TIME) const;

	unsigned long executionEnd = 0;

	int lastResetDayOffset = -1;
};



#endif
