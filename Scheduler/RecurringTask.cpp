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

bool RecurringTask::shouldStart(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	int curDay = SF::evalCurrentDayOfWeek(UNIX_TIME, UNIX_DAY_OFFSET);
	std::vector<bool> wds = SF::daysOfWeekIntToBoolArr(weekdays);
	if(wds[curDay])
	{
		// tag passt schonmal, jetzt noch die uhrzeit pruefen:
		unsigned long curDayBegin = UNIX_TIME - UNIX_TIME % 86400;
		
		unsigned long unixBegin = curDayBegin + begin;
		unsigned long unixEnd	 = unixBegin	 + duration;
		
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
	return getNextStartTime(UNIX_TIME, UNIX_DAY_OFFSET);
}
unsigned long RecurringTask::getEndTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	return getNextEndTime(UNIX_TIME, UNIX_DAY_OFFSET);
}

unsigned long RecurringTask::timeTilNextExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	unsigned long dt = 0;
	
	int curDay = SF::evalCurrentDayOfWeek(UNIX_TIME, UNIX_DAY_OFFSET);
	
	std::vector<bool> wds = SF::daysOfWeekIntToBoolArr(weekdays);
	
	for(int d=curDay; d < curDay + 7; ++d)
	{
		int cwd = d % 7;
		bool willIrrigateToday = false;

		auto curDayTime = (d == curDay) ? UNIX_TIME % 86400 : 0;
		
		if( wds[cwd] && begin >= curDayTime )
		{
			willIrrigateToday = true;
		}
		
		if( willIrrigateToday )
		{
			dt += begin - curDayTime;
		}else{
			auto restTimeOfDay = 86400 - curDayTime;
			dt += restTimeOfDay;
		}
	}
	return dt;
}
unsigned long RecurringTask::timeTilExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	return timeTilNextExecution(UNIX_TIME, UNIX_DAY_OFFSET);
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


