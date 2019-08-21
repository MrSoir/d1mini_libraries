#include "Scheduler.h"


void Scheduler::start(unsigned long exec)
{
	setExecutionEnd(exec);
}
void Scheduler::stop()
{
	executionEnd = 0;
}

void Scheduler::execute(unsigned long UNIX_TIME, int EXECUTION_PIN)
{	 
	if( !isRunning(UNIX_TIME) )
	{
		if(digitalRead(EXECUTION_PIN) == HIGH)
		{
			Serial.println("EXECUTION_PIN -> setting LOW");
			digitalWrite(EXECUTION_PIN, LOW);
		}
	}else{
		if(digitalRead(EXECUTION_PIN) == LOW)
		{
			Serial.println("EXECUTION_PIN -> setting HIGH");
			digitalWrite(EXECUTION_PIN, HIGH);
		}
	}
}

bool Scheduler::isRunning(unsigned long UNIX_TIME)
{
	if(UNIX_TIME >= executionEnd)
	{
		stop();
		return false;
	}else{
/*		Serial.print("is running for: ");
		Serial.print(executionEnd - UNIX_TIME);
		Serial.println("	seconds");*/
		return true;
	}
}
unsigned long Scheduler::isRunningUntil() const
{
	return executionEnd;
}
unsigned long Scheduler::isRunningFor(unsigned long UNIX_TIME) const
{
	if(UNIX_TIME < executionEnd)
	{
		return executionEnd - UNIX_TIME;
	}
	return 0;
}

void Scheduler::update(ScheduleBD& schedule, unsigned long UNIX_TIME, int UNIX_DAY_OFFSET)
{	 
	if( !alreadyResettedToday(UNIX_TIME) )
	{
		resetExecutionStarts(schedule, UNIX_TIME);
	}

	schedule.startEntries(*this, UNIX_TIME, UNIX_DAY_OFFSET);
}

void Scheduler::setExecutionEnd(unsigned long irrigEnd)
{
	if(executionEnd < irrigEnd)
	{
		executionEnd = irrigEnd;
	}
}

void Scheduler::resetExecutionStarts(ScheduleBD& schedule, unsigned long UNIX_TIME)
{
	schedule.resetExecutionStarts();
	
	lastResetDayOffset = UNIX_TIME / 86400;
}
bool Scheduler::alreadyResettedToday(unsigned long UNIX_TIME) const
{
	return (UNIX_TIME / 86400) == lastResetDayOffset;
}
