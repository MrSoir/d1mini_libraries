#include "LEDhandler.h"


LEDHandler::LEDHandler(std::vector<int> dataPins, 
					   std::vector<int> ledCounts,
					   std::shared_ptr<ESP8266WebServer> server)
	: _strips(std::vector<LEDstrip>()),
	  _stripColors(std::vector<CRGB*>()),
	  _ledCounts(ledCounts),
	   server(server)
	  
	//   ,_animation( std::make_shared<StaticLEDanimation>(CRGB(255,0,0)) )
{
	std::vector<CRGB> animColors = {CRGB(255,0,0), CRGB(255,0,255), CRGB(0,0,255)};
	unsigned long animDuration = 1000;

	_animation = std::shared_ptr<SmoothColorTransition>(
					new SmoothColorTransition(animColors, animDuration));

	// _animation = std::shared_ptr<WaveLEDanimation>(new WaveLEDanimation(animColors,
	// 							   CRGB(255,255,255),
	// 						  	   ledCounts,
	// 							   animDuration,
	// 							   0.3,
	// 							   true));
	
	for(int i=0; i < dataPins.size(); ++i)
	{
		_strips.push_back( LEDstrip(ledCounts[i], dataPins[i]) );
		_stripColors.push_back( _strips[i].colors );
	}

	// _animation = std::shared_ptr<ShootingStarAnimation>( 
	// 	new ShootingStarAnimation(animColors, 
	// 					CRGB(0,0,0),
	// 					ledCounts, 
	// 					5000,
	// 					0.2,
	// 					true) );
}


void LEDHandler::setServerURLs()
{
	server->on("/setLEDanimation",				[&](){receiveAndSetAnimation();});

	// example usage in browser:
//	http://192.168.2.110/setAnimation?type=0&parameters...
}

void LEDHandler::receiveAndSetAnimation()
{
	int animationType = server->arg("type").toInt();
	std::shared_ptr<LEDanimation> anim = LEDanimation::generateAnimation(server, _ledCounts);
	if(anim)
	{
		setAnimation(anim);
		server->send(200, "text/plain", "successfully received animation!");
	}else{
		server->send(400, "text/plain", "invalid animation received!");
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
