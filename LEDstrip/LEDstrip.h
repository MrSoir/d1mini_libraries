#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <Arduino.h>

#include <string>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <utility>
#include <map>
#include <set>

#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#include <FastLED.h>

#include <StaticFunctions.h>

class LEDstrip{
public:
	LEDstrip(int ledCount, 
			 int dataPin);

	void setColor(CRGB col);
	
	int ledCount;
	int dataPin;

	CRGB* colors;
};


#endif
