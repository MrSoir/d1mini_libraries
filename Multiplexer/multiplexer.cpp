#include "multiplexer.h"


Multiplexer::Multiplexer(std::vector<int> controlPinIds,
			 std::vector<int> usedAnalogMultiplexerPins,
			 int multiplexerOutputPin)
	: m_controlPinIds(controlPinIds),
	  m_controlPinVals(std::map<int,int>()),
	  
	  m_usedAnalogMultiplexerPins(std::set<int>()),
	  m_sensorVals(std::map<int,int>()),
	  
	  m_multiplexerOutputPin(multiplexerOutputPin)
{
	for(const auto pin: usedAnalogMultiplexerPins)
	{
		m_usedAnalogMultiplexerPins.insert(pin);
		m_sensorVals[pin] = 0;
	}
	for(const auto pin: controlPinIds)
	{
		m_controlPinVals[pin] = 0;

		pinMode(pin, OUTPUT);
	}
	
	pinMode(m_multiplexerOutputPin, INPUT);
}

// evalPins:
// sensorId == 0 => {0,0,0,0}
// sensorId == 1 => {1,0,0,0}
// sensorId == 2 => {0,1,0,0}
// sensorId == 3 => {1,1,0,0}
// sensorId == 4 => {0,0,1,0}
// (...)
// n == 4 => 16:1 Multiplexer => vals: 4-bool-array
// n == 3 =>  8:1 Multiplexer => vals: 3-bool-array
// n == 2 =>  4:1 Multiplexer => vals: 2-bool-array
void Multiplexer::evalControlPinVals(int analogPinId)
{
  for(int i=0; i < m_controlPinIds.size(); ++i)
  {
    int div = SF::powInt(2, i);
    m_controlPinVals[m_controlPinIds[i]] = (analogPinId / div) % 2;
  }
}

void Multiplexer::setControlPin(int controlPinId)
{
	if(m_controlPinVals[controlPinId] == 0)
	{
		digitalWrite(controlPinId, LOW);
	}else{
		digitalWrite(controlPinId, HIGH);
	}
}
void Multiplexer::setControlPins()
{
	for(const auto controlPinId: m_controlPinIds)
	{
		setControlPin(controlPinId);
	}
}

void Multiplexer::readAnalogPins()
{
	for(const auto analogPinId: m_usedAnalogMultiplexerPins)
	{
		evalControlPinVals(analogPinId);
		setControlPins();
		
		m_sensorVals[analogPinId] = analogRead(m_multiplexerOutputPin);
		
		delay(1);
	}
}
std::vector<std::pair<int,int>> Multiplexer::getSensorValues()
{
    return genValues();
}
int Multiplexer::getSensorValue(int analogSensorId)
{
    return m_sensorVals[analogSensorId];
}
std::vector<std::pair<int,int>> Multiplexer::genValues()
{
	std::vector<std::pair<int,int>> vals;
	for(const auto analogPin: m_usedAnalogMultiplexerPins)
	{
		vals.push_back( std::make_pair(analogPin, m_sensorVals[analogPin]) );
	}
	return vals;
}

void Multiplexer::setControlPinIds(std::vector<int> controlPinIds)
{
	m_controlPinIds = controlPinIds;
}
void Multiplexer::setUsedAnalogMultiplexerPins(const std::vector<int>& usedAnalogMultiplexerPins)
{
	m_usedAnalogMultiplexerPins.clear();
	for(const auto analogPin: usedAnalogMultiplexerPins)
	{
		m_usedAnalogMultiplexerPins.insert(analogPin);
	}
}

void Multiplexer::setMultiplexerOutputPin(int multiplexerOutputPin)
{
	m_multiplexerOutputPin = multiplexerOutputPin;
	pinMode(m_multiplexerOutputPin, INPUT);
}

void Multiplexer::addSensor(int analogMultiplexerPin)
{
	m_usedAnalogMultiplexerPins.insert(analogMultiplexerPin);
}
bool Multiplexer::removeSensor(int analogMultiplexerPin)
{
	auto it = m_usedAnalogMultiplexerPins.find(analogMultiplexerPin);
	if(it != m_usedAnalogMultiplexerPins.end())
	{
		m_usedAnalogMultiplexerPins.erase(it);
		return true;
	}
	return false;
}

void Multiplexer::clearSensors()
{
    m_usedAnalogMultiplexerPins.clear();
}
