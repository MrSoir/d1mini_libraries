#ifndef Multiplexer_h
#define Multiplexer_h

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

class Multiplexer{
public:
	// controlPinCount: maximum is 4!!! -> enables to connect to 16 analog sensors
	// controlPinCount == 4 => 16:1 Multiplexer
	// controlPinCount == 3 =>  8:1 Multiplexer
	// controlPinCount == 2 =>  4:1 Multiplexer
	Multiplexer(std::vector<int> controlPinIds, 
				std::vector<int> usedAnalogMultiplexerPins,
				int multiplexerOutputPin);

	void readAnalogPins();
	std::vector<std::pair<int,int>> getSensorValues();
	int getSensorValue(int analogSensorId);

	void setControlPinIds(std::vector<int> controlPinIds);
	void setUsedAnalogMultiplexerPins(const std::vector<int>& usedAnalogMultiplexerPins);
	void setMultiplexerOutputPin(int multiplexerOutputPin);

	void addSensor(int analogMultiplexerPin);
	bool removeSensor(int analogMultiplexerPin);
	void clearSensors();

private:
   	std::vector<std::pair<int,int>> genValues();
    
	void evalControlPinVals(int analogPinId);
	void setControlPin(int id);
	void setControlPins();
	
	std::vector<int> m_controlPinIds;
	std::map<int,int> m_controlPinVals;
	
	std::set<int> m_usedAnalogMultiplexerPins;
	int m_multiplexerOutputPin;

	std::map<int, int> m_sensorVals;
};

#endif
