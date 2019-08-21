#include "ScheduleBD.h"


ScheduleBD::ScheduleBD()
	: _entries(std::vector<Task*>())
{}

void ScheduleBD::add(Task* ie)
{
	this->_entries.push_back(ie);
}
void ScheduleBD::remove(Task* ref_ie)
{
	auto id = indexOf(ref_ie);
	if(id > -1)
	{
		auto* ie = _entries[id];
		_entries.erase( _entries.begin() + static_cast<unsigned int>(id) );
		if(_entries.size() > 0)
		{
			_entries.shrink_to_fit();
		}
		delete ie;
	}
	delete ref_ie;
}
void ScheduleBD::clear()
{
	std::vector<Task*>().swap( _entries );
}

int ScheduleBD::indexOf(const Task* ie) const
{
	for(int i=0; i < _entries.size(); ++i)
	{
		if( *ie == *(_entries[i]) )
		{
			return i;
		}
	}
	return -1;
}
bool ScheduleBD::contains(const Task* ie) const
{
	return indexOf(ie) > -1;
}
const std::vector<Task*>& ScheduleBD::entries() const
{
	return _entries;
}

void ScheduleBD::startEntries(Scheduler& scheduler, unsigned long UNIX_TIME, int UNIX_DAY_OFFSET)
{
	for(int i=0; i < _entries.size(); ++i)
	{
		if(_entries[i]->shouldStart(UNIX_TIME, UNIX_DAY_OFFSET) && !_entries[i]->started)
		{
			startTask(_entries[i], scheduler, UNIX_TIME, UNIX_DAY_OFFSET);
		}
	}
	this->removeExpiredEntries(UNIX_TIME, UNIX_DAY_OFFSET);
}
void ScheduleBD::removeExpiredEntries(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET)
{
	for(int i=0; i < _entries.size(); )
	{
		auto* ie = _entries[i];
		if(ie->getType() == 1 && ie->getEndTime(UNIX_TIME, UNIX_DAY_OFFSET) < UNIX_TIME)
		{
			Serial.print("deleting expired ie: ");
			ie->println();
			
			_entries.erase(_entries.begin() + i);
			delete ie;
		}else{
			++i;
		}
	}
}
void ScheduleBD::resetExecutionStarts()
{
	for(int i=0; i < _entries.size(); ++i)
	{
		_entries[i]->started = false;
	}
}
void ScheduleBD::startTask(Task* ie, Scheduler& scheduler, unsigned long UNIX_TIME, int UNIX_DAY_OFFSET)
{
	auto endTime = ie->getEndTime(UNIX_TIME, UNIX_DAY_OFFSET);
	ie->started = true;
	scheduler.setExecutionEnd( endTime );
	Serial.print("ScheduleBD::startTask -> starting task: endTime: ");
	Serial.print(endTime);
	Serial.print("	UNIX_TIME: ");
	Serial.print(UNIX_TIME);
	Serial.print("	diff: ");
	Serial.println(endTime - UNIX_TIME);
}

unsigned long ScheduleBD::timeTilNextExecution(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET) const
{
	unsigned long dt = 0;
	
	bool alrdySet = false;
	
	for(const auto* ie: _entries)
	{
		auto cur_dt = ie->timeTilExecution(UNIX_TIME, UNIX_DAY_OFFSET);
		
		if(!alrdySet || dt > cur_dt)
		{
			alrdySet = true;
			dt = cur_dt;
		}
	}
	return dt;
}

String ScheduleBD::createArduinoStringFromAllEntries() const
{
	String str("");
	unsigned int cntr = 0;
	for(auto it=_entries.begin(); it < _entries.end(); ++it)
	{
		str.concat( (*it)->toArduinoString() );
		if(cntr + 1 < _entries.size())
		{
			str.concat(String("|"));
		}
		++cntr;
	}
	return str;
}

void ScheduleBD::printEntries() const
{
  Serial.println("\nScheduleBD:");
  for(auto it = _entries.begin(); it < _entries.end(); ++it)
  {
    (*it)->println();
  }
  Serial.println();
}


