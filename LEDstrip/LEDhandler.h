#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include <Arduino.h>

#include <string>
#include <vector>
#include <math.h>
#include <tuple>
#include <algorithm>
#include <utility>
#include <map>
#include <set>
#include <memory>

#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#include <FastLED.h>

#include <StaticFunctions.h>
#include <LEDstrip.h>
#include <LEDanimation.h>

class LEDHandler{
public:
	LEDHandler(std::vector<int> dataPins, 
			   std::vector<int> ledCounts);
	
	void setAnimation(std::shared_ptr<LEDanimation> anim);

	const std::vector<CRGB*>& getColors() const;


	void update(unsigned long dt);

private:
	std::vector<LEDstrip> 	_strips;
	std::vector<CRGB*>		_stripColors; // keep reference for fast lookup
	std::vector<int>  		_ledCounts;
	
	std::shared_ptr<LEDanimation> _animation;
};


#endif
