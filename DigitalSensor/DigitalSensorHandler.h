#ifndef DIGITAL_SENSOR_HANDLER_H
#define DIGITAL_SENSOR_HANDLER_H

#include <Arduino.h>

#include <string>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <utility>
#include <map>
#include <set>

#include <StaticFunctions.h>
#include <DigitalSensor.h>

class DigitalSensorHandler{
public:
	DigitalSensorHandler();
	
	bool anySensorActive() const;
	void addSensor(int sensorPin);
	void removeSensor(int sensorPin);
	void clearSensors();
	bool containsSensor(int sensorPin) const;

private:
	void _removeSensorHlpr(int sensorPin);
	
	std::vector<DigitalSensor> _sensors;	
	std::set<int> _sensorPins;
};


#endif
