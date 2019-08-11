#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

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
			   std::vector<int> ledCounts,
			   std::shared_ptr<ESP8266WebServer> server);

	void setServerURLs();
	
	void setAnimation(std::shared_ptr<LEDanimation> anim);

	const std::vector<CRGB*>& getColors() const;


	void update(unsigned long dt);

private:
	void receiveAndSetAnimation();

	std::vector<LEDstrip> 	_strips;
	std::vector<CRGB*>		_stripColors; // keep reference for fast lookup
	std::vector<int>  		_ledCounts;
	
	std::shared_ptr<LEDanimation> _animation;

	std::shared_ptr<ESP8266WebServer> server;
};


#endif
