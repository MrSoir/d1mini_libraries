#ifndef LED_ANIMATION_H
#define LED_ANIMATION_H

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
};

//-------------------------------------------------------------

class StaticLEDanimation: public LEDanimation{
public:
	StaticLEDanimation(CRGB col);
	
	virtual void update(std::vector<CRGB*>& strips, 
						std::vector<int>& ledCounts, 
						unsigned long dt) override;
	
	void setColor(CRGB col);

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
					 float baseBrightness,
					 std::vector<int> ledCounts,
					 unsigned long duration,
					 float range,
				 	 bool synchronize=false);
	
	virtual void update(std::vector<CRGB*>& strips, 
						std::vector<int>& ledCounts, 
						unsigned long dt) override;
	
	void setWaveColors(std::vector<CRGB> cols);
	void setSynchronized(bool synchronize);

private:
	std::pair<int,int> evalArrId(float absId);
	
	CRGB getFirstColor();
	CRGB getSecondColor();
	CRGB evalCurColor();

	CRGB evalRangeColor(float relProg, CRGB rngCol, CRGB lastCol);

	float evalFirstId(int ledsCnt);
	float evalRange(int ledsCnt);
	
	void evalTotalLEDcount();
	
	std::vector<CRGB> _waveColors;
	float _baseBrightness = 0.3;
	int _curColId = 0;
	
	std::vector<int> _ledCounts;
	
	bool _synchronize;

	float _progress = 0;
	float _range = 0.2;
	unsigned long _duration = 5000;

	int _totalLEDcount = 0;
};

#endif
