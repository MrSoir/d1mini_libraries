#ifndef ANALOG_SENSOR_PIN_ENTRY_H
#define ANALOG_SENSOR_PIN_ENTRY_H

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include <StaticFunctions.h>

#include <string>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <utility>
#include <map>
#include <set>
#include <memory>

#include <StaticFunctions.h>


//---------------------------------------------------

enum SensorType{
  MOISTURE_SENSOR,
  TEMP_SENSOR,
  LIGHT_SENSOR,
  ANY
};

//---------------------------------------------------

class AnalogSensorPinEntry{
public:
  AnalogSensorPinEntry(String id,
                       int pin,
                       float sensitivity=0.5,
                       float value=0.0,
                       float minAbsValue=0.0,
                       float maxAbsValue=1000.0,
                       SensorType type=SensorType::MOISTURE_SENSOR);
  
  String id;
  int pin;
  float sensitivity;
private:
  float value;
  float minAbsValue;
  float maxAbsValue;
public:
  SensorType type;
  bool triggerIfSensorValueGreaterThanThreshold = true;


//-------------
  void setMinAbsValue(float minAbsValue);
  void setMinAbsValue(int minAbsValue);
  
  void setMaxAbsValue(float maxAbsValue);
  void setMaxAbsValue(int maxAbsValue);
//-------------
  void setAbsValue(float value);
  void setAbsValue(int value);
//-------------
  void setRelValue(float value);
  void setRelValue(int value);
//-------------
  float absValue() const;
  float relValue() const;
//-------------
  bool shallTrigger() const;
};

#endif
