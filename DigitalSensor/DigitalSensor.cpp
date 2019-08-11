#include "DigitalSensor.h"


DigitalSensor::DigitalSensor(int pin)
	: pin(pin)
{
	auto delayBuff = 50;
	
	pinMode(pin, OUTPUT);
		delay(delayBuff);

	digitalWrite(pin, LOW);
		delay(delayBuff);
	
	pinMode(pin, INPUT);
		delay(delayBuff);
}

bool DigitalSensor::isActive() const
{
	return (digitalRead(pin) == HIGH);
}
