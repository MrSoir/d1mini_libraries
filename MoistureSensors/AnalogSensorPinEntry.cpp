#include "AnalogSensorPinEntry.h"


AnalogSensorPinEntry::AnalogSensorPinEntry(String id,
											 int pin,
											 float sensitivity,
											 float value,
											 float minAbsValue,
											 float maxAbsValue,
											 SensorType type)
	: id(id),
		pin(pin),
		sensitivity(sensitivity),
		value(value),
		minAbsValue(minAbsValue),
		maxAbsValue(maxAbsValue),
		type(type)
{}

//-------------
void AnalogSensorPinEntry::setMinAbsValue(float minAbsValue){
	this->minAbsValue = minAbsValue;
}
void AnalogSensorPinEntry::setMinAbsValue(int minAbsValue){
	auto minValueflt = static_cast<float>(minAbsValue);
	this->setMinAbsValue(minValueflt);
}

void AnalogSensorPinEntry::setMaxAbsValue(float maxAbsValue){
	this->maxAbsValue = maxAbsValue;
}
void AnalogSensorPinEntry::setMaxAbsValue(int maxAbsValue){
	auto maxValueflt = static_cast<float>(maxAbsValue);
	this->setMaxAbsValue(maxValueflt);
}
//-------------
float AnalogSensorPinEntry::mapAbsValue(float absValue)
{
	absValue = absValue < minAbsValue ? minAbsValue : absValue;
	absValue = absValue > maxAbsValue ? maxAbsValue : absValue;
	return absValue;
}
void AnalogSensorPinEntry::setAbsValue(float absValue){
	absValue = mapAbsValue(absValue);
	this->value = (absValue - minAbsValue) / (maxAbsValue - minAbsValue);
}
void AnalogSensorPinEntry::setAbsValue(int value){
	float valueflt = static_cast<float>(value);
	this->setAbsValue(valueflt);
}
//-------------
void AnalogSensorPinEntry::setRelValue(float value){
	this->value = value;
}
void AnalogSensorPinEntry::setRelValue(int value){
	float valueflt = static_cast<float>(value);
	this->setRelValue(valueflt);
}
//-------------
float AnalogSensorPinEntry::absValue() const{
	return minAbsValue + (maxAbsValue - minAbsValue ) * value;
}
float AnalogSensorPinEntry::relValue() const{
	return value;
}
//-------------
bool AnalogSensorPinEntry::shallTrigger() const {
	return triggerIfSensorValueGreaterThanThreshold
			? value > sensitivity
			: value < sensitivity;
}



