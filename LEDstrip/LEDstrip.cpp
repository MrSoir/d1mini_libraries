#include "LEDstrip.h"


LEDstrip::LEDstrip(int ledCount, 
				   int dataPin)
	: ledCount(ledCount),
	  dataPin(dataPin),

	  colors(new CRGB[ledCount])
{
	//FastLED.addLeds<type, dataPin>(colors, ledCount);
}


void LEDstrip::setColor(CRGB col)
{
	for(int i=0; i < ledCount; ++i)
	{
		colors[i] = col;
	}
}
