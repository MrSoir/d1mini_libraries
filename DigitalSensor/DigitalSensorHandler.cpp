#include "DigitalSensorHandler.h"

DigitalSensorHandler::DigitalSensorHandler()
	: _sensors(std::vector<DigitalSensor>()),
	  _sensorPins(std::set<int>())
{
}

bool DigitalSensorHandler::anySensorActive() const
{
	for(const auto& sens: _sensors)
	{
		if(sens.isActive())
		{
			return true;
		}
	}
	return false;
}
void DigitalSensorHandler::addSensor(int sensorPin)
{
	if( !containsSensor(sensorPin) )
	{
		DigitalSensor sens(sensorPin);
		_sensors.push_back(sens);
		_sensorPins.insert(sensorPin);
	}
}
void DigitalSensorHandler::removeSensor(int sensorPin)
{
	if( !containsSensor(sensorPin) )
	{
		_removeSensorHlpr(sensorPin);
		_sensorPins.erase( _sensorPins.find(sensorPin) );
	}
}
void DigitalSensorHandler::_removeSensorHlpr(int sensorPin)
{
	for(auto it=_sensors.begin(); it != _sensors.end(); ++it)
	{
		if( (*it).pin == sensorPin )
		{
			_sensors.erase(it);
			return;
		}
	}
}
void DigitalSensorHandler::clearSensors()
{
	_sensors.clear();
	_sensorPins.clear();
}
bool DigitalSensorHandler::containsSensor(int sensorPin) const
{
	return _sensorPins.find(sensorPin) != _sensorPins.end();
}
