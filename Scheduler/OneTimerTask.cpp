#include "OneTimerTask.h"

OneTimerTask::OneTimerTask(unsigned long begin,
					   int duration)
	: begin(begin),
	  duration(duration)
{
	this->type = 1;
}

int OneTimerTask::getType() const
{
	return this->type;
}

bool OneTimerTask::operator==(const Task& ie) const
{
	if(ie.getType() == 1)
	{
		const auto& ot = static_cast<const OneTimerTask&>(ie);
		return this->begin		== ot.begin &&
					 this->duration == ot.duration;
	}
	return false;
} 

bool OneTimerTask::shouldStart(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	return UNIX_TIME > begin;
}

unsigned long OneTimerTask::getStartTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	return begin;
}
unsigned long OneTimerTask::getEndTime(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	return begin + duration;
}

unsigned long OneTimerTask::timeTilExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	return begin > UNIX_TIME ? begin - UNIX_TIME : 0;
}

String OneTimerTask::toArduinoString() const
{
	String str(getType());
	str.concat(String("-"));
	str.concat(begin);
	str.concat(String("-"));
	str.concat(duration);
	return str;
}

void OneTimerTask::print() const
{
	Serial.print("OneTimerTask: begin: ");
	Serial.print(begin);
	Serial.print("	duration: ");
	Serial.print(duration);
}
void OneTimerTask::println() const
{
	print();
	Serial.println();
}


