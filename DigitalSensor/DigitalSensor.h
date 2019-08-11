#ifndef DIGITAL_SENSOR_H
#define DIGITAL_SENSOR_H

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

class DigitalSensor{
public:
	DigitalSensor(int pin);
	
	bool isActive() const;
	
	int pin;	
};


#endif
