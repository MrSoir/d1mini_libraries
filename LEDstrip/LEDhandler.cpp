#include "LEDhandler.h"


LEDHandler::LEDHandler(std::vector<int> dataPins, 
					   std::vector<int> ledCounts)
	: _strips(std::vector<LEDstrip>()),
	  _stripColors(std::vector<CRGB*>()),
	  _ledCounts(ledCounts)
	  
	  //_animation( std::make_shared<StaticLEDanimation>(CRGB(255,0,0)) )
	  //_animation( std::make_shared<SmoothColorTransition>(, 5000) )
{
	std::vector<CRGB> animColors = {CRGB(255,0,0), CRGB(0,255,0), CRGB(0,100,0), CRGB(0,0,255)};
	unsigned long animDuration = 1000;
	_animation = std::shared_ptr<SmoothColorTransition>(new SmoothColorTransition(animColors, animDuration));
	
	for(int i=0; i < dataPins.size(); ++i)
	{
		_strips.push_back( LEDstrip(ledCounts[i], dataPins[i]) );
		_stripColors.push_back( _strips[i].colors );
	}
}


void LEDHandler::setAnimation(std::shared_ptr<LEDanimation> anim)
{
	_animation = anim;
}

const std::vector<CRGB*>& LEDHandler::getColors() const
{
	return _stripColors;
}


void LEDHandler::update(unsigned long dt)
{
	_animation->update(_stripColors, _ledCounts, dt);
}
