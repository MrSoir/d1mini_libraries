#include "RecurringTask.h"

//----------------------RecurringTask implementation----------------------

RecurringTask::RecurringTask(int weekdays,
						int begin,
						int duration)
	: weekdays(weekdays),
		begin(begin),
		duration(duration)
{
	this->type = 0;
}
RecurringTask::RecurringTask(JsonArray weekdays,
						int begin,
						int duration)
	: weekdays(SF::arrayToDaysWofWeek(weekdays)),
		begin(begin),
		duration(duration)
{
	this->type = 0;
}

int RecurringTask::getType() const
{
	return this->type;
}

bool RecurringTask::operator==(const Task& ie) const
{
	if(ie.getType() == 0)
	{
		const auto& re = static_cast<const RecurringTask&>(ie);
		return this->weekdays	== re.weekdays &&
					 this->begin		 == re.begin &&
					 this->duration	== re.duration;
	}
	return false;
}

bool RecurringTask::executesThisCurrentDay(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	int curDay = SF::evalCurrentDayOfWeek(UNIX_TIME, UNIX_DAY_OFFSET);
	std::vector<bool> wds = SF::daysOfWeekIntToBoolArr(weekdays);
	return wds[curDay];
}
bool RecurringTask::shouldStart(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	bool executesToday = executesThisCurrentDay(UNIX_TIME, UNIX_DAY_OFFSET);
	if(executesToday)
	{
		// tag passt schonmal, jetzt noch die uhrzeit pruefen:
		unsigned long curDayBegin = SF::evalCurrentDayBegin(UNIX_TIME);//UNIX_TIME - UNIX_TIME % 86400;
		
		unsigned long unixBegin = curDayBegin + begin;
		unsigned long unixEnd	= unixBegin	 + duration;
		
		if(UNIX_TIME >= unixBegin &&
		   UNIX_TIME <	unixEnd)
		{
			return true;
		}
	}
	return false;
}
unsigned long RecurringTask::getNextStartTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	return UNIX_TIME + timeTilNextExecution(UNIX_TIME, UNIX_DAY_OFFSET);
}
unsigned long RecurringTask::getNextEndTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	return getNextStartTime(UNIX_TIME, UNIX_DAY_OFFSET) + duration;
}

unsigned long RecurringTask::getStartTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	// difference between getStartTime and getNextStartTime:
	//	if task is currently supposed to run (curBegin <= UNIX_TIME <= curEnd)
	// 	then getStartTime lies in the past!
	// whereas getNextStartTime always seraches for the next start > UNIX_TIME!!
	if(	shouldStart(UNIX_TIME, UNIX_DAY_OFFSET) )
	{
		unsigned long curDayBegin = SF::evalCurrentDayBegin(UNIX_TIME);//UNIX_TIME - UNIX_TIME % 86400;
		
		unsigned long unixBegin = curDayBegin + begin;
		
		return unixBegin;
	}else{
		return getNextStartTime(UNIX_TIME, UNIX_DAY_OFFSET);
	}
}
unsigned long RecurringTask::getEndTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	if(	shouldStart(UNIX_TIME, UNIX_DAY_OFFSET) )
	{
		unsigned long curDayBegin = SF::evalCurrentDayBegin(UNIX_TIME);//UNIX_TIME - UNIX_TIME % 86400;
		
		unsigned long unixBegin = curDayBegin + begin;
		unsigned long unixEnd	= unixBegin	 + duration;
		
		return unixEnd;
	}else{
		return getNextEndTime(UNIX_TIME, UNIX_DAY_OFFSET);
	}
}

bool RecurringTask::alreadyExecutedToday(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	auto wds = SF::daysOfWeekIntToBoolArr(weekdays);
	auto curDay = SF::evalCurrentDayOfWeek(UNIX_TIME, UNIX_DAY_OFFSET);
	
	unsigned long curDayBegin = SF::evalCurrentDayBegin(UNIX_TIME);//UNIX_TIME - UNIX_TIME % 86400;
	
	return wds[curDay] && curDayBegin >= begin;
}
unsigned long RecurringTask::timeTilNextExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	if( shouldStart(UNIX_TIME, UNIX_DAY_OFFSET) )
	{
		return 0;
	}

	unsigned long dt = 0;

	if( executesThisCurrentDay(UNIX_TIME, UNIX_DAY_OFFSET) )
	{
		unsigned long curDayBegin = SF::evalCurrentDayBegin(UNIX_TIME);//UNIX_TIME - UNIX_TIME % 86400;
		
		if( alreadyExecutedToday(UNIX_TIME, UNIX_DAY_OFFSET) )
		{
			dt += 86400 - curDayBegin;
		}else{
			return begin - curDayBegin;
		}
	}

	auto wds = SF::daysOfWeekIntToBoolArr(weekdays);
	auto curWeekDay = (SF::evalCurrentDayOfWeek(UNIX_TIME, UNIX_DAY_OFFSET) + 1) % 7;

	for(int i=0; i < 7; ++i){
		if( wds[curWeekDay] ){
			dt += begin;
			return dt;
		}else{
			dt += 86400;
		}
		++curWeekDay;
	}
	
	return dt;
}
unsigned long RecurringTask::timeTilExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	if(	shouldStart(UNIX_TIME, UNIX_DAY_OFFSET) )
	{
		return 0;
	}else{
		return timeTilNextExecution(UNIX_TIME, UNIX_DAY_OFFSET);
	}
}

String RecurringTask::toArduinoString() const
{
	String str(getType());
	str.concat(String("-"));
	str.concat(weekdays);
	str.concat(String("-"));
	str.concat(begin);
	str.concat(String("-"));
	str.concat(duration);
	return str;
}

void RecurringTask::print() const
{
	Serial.print("RecurringTask: weekdays: ");
	Serial.print(weekdays);
	Serial.print("	begin: ");
	Serial.print(begin);
	Serial.print("	duration: ");
	Serial.print(duration);
}
void RecurringTask::println() const
{
	print();
	Serial.println();
}


