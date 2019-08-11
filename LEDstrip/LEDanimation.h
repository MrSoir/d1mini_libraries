#ifndef LED_ANIMATION_H
#define LED_ANIMATION_H

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

#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#include <FastLED.h>

#include <StaticFunctions.h>


//-------------------------------------------------------------

enum LEDAnimationType{
	STATIC,
	WAVE,
	FADE
};

//-------------------------------------------------------------

class LEDanimation{
public:
	// dt: delayed time since last update
	virtual void update(std::vector<CRGB*>& colors, 
						std::vector<int>& colorCounts, 
						unsigned long dt) = 0;

	virtual String toString() const = 0;

	static std::shared_ptr<LEDanimation> generateAnimation(std::shared_ptr<ESP8266WebServer> server, 
						std::vector<int>& ledCounts);
};

//-------------------------------------------------------------

class StaticLEDanimation: public LEDanimation{
public:
	StaticLEDanimation(CRGB col);
	
	virtual void update(std::vector<CRGB*>& strips, 
						std::vector<int>& ledCounts, 
						unsigned long dt) override;

	static std::shared_ptr<LEDanimation> generateAnimation(std::shared_ptr<ESP8266WebServer> server);
	
	void setColor(CRGB col);

	virtual String toString() const override{
		String str("StaticLEDanimation");
		return str;
	}

private:
	CRGB _col;
	bool _colChanged = true;
};

//-------------------------------------------------------------

class SmoothColorTransition: public LEDanimation{
public:
	SmoothColorTransition(std::vector<CRGB> colors,
						  unsigned long duration);

	virtual void update(std::vector<CRGB*>& strips, 
						std::vector<int>& ledCounts, 
						unsigned long dt) override;

	void setColors(std::vector<CRGB> colors);
	void setDuration(unsigned long duration);

	static std::shared_ptr<LEDanimation> generateAnimation(std::shared_ptr<ESP8266WebServer> server);
	
	virtual String toString() const override{
		String str("SmoothColorTransition");
		return str;
	}
private:
	CRGB curColor() const;
	CRGB nextColor() const;
	
	std::vector<CRGB> _colors;
	unsigned long _progress = 0;
	unsigned long _duration;

	int _curColorId = 0;
	
	bool _colChanged = true;
};

//-------------------------------------------------------------

class WaveLEDanimation: public LEDanimation{
public:
	WaveLEDanimation(std::vector<CRGB> waveColors,
					 CRGB peakColor,
					 std::vector<int> ledCounts,
					 unsigned long duration,
					 float range,
				 	 bool synchronize=false);
	
	virtual void update(std::vector<CRGB*>& strips, 
						std::vector<int>& ledCounts, 
						unsigned long dt) override;
	
	void setWaveColors(std::vector<CRGB> cols);
	void setSynchronized(bool synchronize);

	static std::shared_ptr<LEDanimation> generateAnimation(std::shared_ptr<ESP8266WebServer> server, 
							std::vector<int>& ledCounts);

	virtual String toString() const override{
		String str("WaveLEDanimation");
		return str;
	}

private:
	CRGB getFirstColor();
	CRGB getSecondColor();
	CRGB getLastColor();
	CRGB evalCurColor();

	float evalFirstId();
	float evalRange();
	
	std::vector<CRGB> _waveColors;
	CRGB _peakColor = CRGB(255,255,255);
	int _curColId = 0;
	
	std::vector<int> _ledCounts;
	
	bool _synchronize;

	float _progress = 0;
	float _range = 0.2;
	unsigned long _duration = 5000;

	int _totalLEDcount = 0;
};


//-------------------------------------------------------------

class ShootingStarAnimation: public LEDanimation{
public:
	ShootingStarAnimation(std::vector<CRGB> colors,
						  CRGB baseCol,
						  std::vector<int> ledCounts,
						  unsigned long duration,
						  float range,
						  bool synchronize=true);
	
	virtual void update(std::vector<CRGB*>& strips, 
						std::vector<int>& ledCounts, 
						unsigned long dt) override;
	
	void setAnimColors(std::vector<CRGB> colors);
	void setSynchronized(bool synchronize);

	static std::shared_ptr<LEDanimation> generateAnimation(std::shared_ptr<ESP8266WebServer> server, 
								std::vector<int>& ledCounts);

	virtual String toString() const override{
		String str("ShootingStarAnimation");
		return str;
	}

private:
	void sinelon(CRGB tarCol, CRGB* strip, int ledCount);

	CRGB getFirstColor();
	CRGB getSecondColor();
	CRGB evalCurColor();

	std::vector<CRGB> _animCols;
	int _curColId = 0;
	CRGB _baseCol;
	
	std::vector<int> _ledCounts;
	
	bool _synchronize;

	float _progress = 0;
	float _range = 0.2;
	unsigned long _duration = 5000;

	int _totalLEDcount = 0;
};

#endif
