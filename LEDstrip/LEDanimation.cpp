#include "LEDanimation.h"


//------------------------------StaticFunctions------------------------------

void printColor(CRGB col)
{
	Serial.print("Col(");
	Serial.print(col.r);
	Serial.print(",");
	Serial.print(col.g);
	Serial.print(",");
	Serial.print(col.b);
	Serial.print(")");
}

void setColorToLEDs(CRGB color, std::vector<CRGB*>& strips, std::vector<int>& ledCounts)
{
	for(int i=0; i < strips.size(); ++i)
	{
		for(int j=0; j < ledCounts[i]; ++j)
		{
			strips[i][j] = color;
		}
	}
}

CRGB composeCol(const CRGB& col0, const CRGB& col1, float perctg)
{
	auto drf 	= static_cast<float>(col1.r - col0.r);//colDiff.red);
	auto dgf 	= static_cast<float>(col1.g - col0.g);//colDiff.green);
	auto dbf 	= static_cast<float>(col1.b - col0.b);//colDiff.blue);

	int dr = static_cast<float>(drf  * perctg);
	int dg = static_cast<float>(dgf  * perctg);
	int db = static_cast<float>(dbf  * perctg);

	CRGB tarCol(col0.r + dr,
				col0.g + dg,
				col0.b + db);
	
	// Serial.println();
	// Serial.print("col0: ");
	// printColor(col0);
	// Serial.print("	col1: ");
	// printColor(col1);
	// Serial.print("	perctg: ");
	// Serial.print(perctg);
	// Serial.print("	dr: ");
	// Serial.print(dr);
	// Serial.print("	dg: ");
	// Serial.print(dg);
	// Serial.print("	db: ");
	// Serial.print(db);
	// Serial.print("	tarCol: ");
	// printColor(tarCol);
	// Serial.println();
	
	return tarCol;
}

//------------------------------LEDanimation------------------------------


std::shared_ptr<LEDanimation> LEDanimation::generateAnimation(std::shared_ptr<ESP8266WebServer> server,
			std::vector<int>& ledCounts)
{
	int type = server->arg("type").toInt();

	std::shared_ptr<LEDanimation> anim = nullptr;
	if(type == 0){
		anim = StaticLEDanimation::generateAnimation(server);
	}else if(type == 1){
		anim = SmoothColorTransition::generateAnimation(server);
	}else if(type == 2){
		anim = WaveLEDanimation::generateAnimation(server, ledCounts);
	}else if(type == 3){
		anim = ShootingStarAnimation::generateAnimation(server, ledCounts);
	}

	if(anim)
	{
		Serial.print("genearteAnimation: ");
		Serial.println(anim->toString());
	}

	return anim;
}

CRGB crgbFromHexColorString(const String& hexColStr)
{
	auto colVals =  SF::hexColorStringToULongs(hexColStr);

	uint8_t cv0 = static_cast<uint8_t>( std::get<0>(colVals) );
	uint8_t cv1 = static_cast<uint8_t>( std::get<1>(colVals) );
	uint8_t cv2 = static_cast<uint8_t>( std::get<2>(colVals) );

	Serial.print("	cv0: ");
	Serial.print(cv0);
	Serial.print("	cv1: ");
	Serial.print(cv1);
	Serial.print("	cv2: ");
	Serial.println(cv2);

	return CRGB(cv0, cv1, cv2);
}

std::vector<CRGB> colorsStringToColorsVec(const String& colorsStr)
{
	auto spltCols = SF::splitString(colorsStr, '-');

	std::vector<CRGB> colors;
	for(const auto& colStr: spltCols)
	{
		auto col = crgbFromHexColorString(colStr);

		colors.push_back( col );
	}
	return colors;
}

std::shared_ptr<LEDanimation> StaticLEDanimation::generateAnimation(std::shared_ptr<ESP8266WebServer> server)
{
	CRGB col = crgbFromHexColorString( server->arg("color") );
	return std::make_shared<StaticLEDanimation>(col);
}
std::shared_ptr<LEDanimation> SmoothColorTransition::generateAnimation(std::shared_ptr<ESP8266WebServer> server)
{
	unsigned long duration = server->arg("duration").toInt();

	auto colors = colorsStringToColorsVec( server->arg("colors") );
	
	return std::make_shared<SmoothColorTransition>(colors, duration);
}
std::shared_ptr<LEDanimation> WaveLEDanimation::generateAnimation(std::shared_ptr<ESP8266WebServer> server,
					std::vector<int>& ledCounts)
{
	unsigned long duration = server->arg("duration").toInt();

	Serial.print("WaveLEDanimation: ");

	Serial.print("	duration: ");
	Serial.println(duration);

	bool synchronize = true;//server->arg("synchronize").toInt();

	float range = server->arg("range").toFloat();

	String colorsStr = server->arg("colors");
	auto colors = colorsStringToColorsVec( colorsStr );

	CRGB peakColor = CRGB(255,255,255);//crgbFromHexColorString( server->arg("peakColor") );
	
	return std::make_shared<WaveLEDanimation>(colors, peakColor, ledCounts, duration, range, synchronize);
}
std::shared_ptr<LEDanimation> ShootingStarAnimation::generateAnimation(std::shared_ptr<ESP8266WebServer> server,
					std::vector<int>& ledCounts)
{
	unsigned long duration = server->arg("duration").toInt();

	bool synchronize = true;//server->arg("synchronize").toInt();

	float range = server->arg("range").toFloat();

	String colorsStr = server->arg("colors");
	auto colors = colorsStringToColorsVec( colorsStr );

	CRGB baseColor = CRGB(0,0,0);
		
	return std::make_shared<ShootingStarAnimation>(colors, baseColor, ledCounts, duration, range, synchronize);
}

//------------------------------StaticLEDanimation------------------------------

void evalTotalLEDcount(int* totalLEDcount, std::vector<int> ledCounts)
{
	*totalLEDcount = 0;
	for(int i=0; i < ledCounts.size(); ++i)
	{
		*totalLEDcount += ledCounts[i];
	}
}

std::pair<int,int> evalArrId(float absId, std::vector<int>& _ledCounts)
{
	int cumulatedId = 0;
	for(int i=0; i < _ledCounts.size(); ++i)
	{
		int curArrSize = _ledCounts[i];
		if(static_cast<float>(cumulatedId + curArrSize) > absId)
		{
			// targetArr found -> now search for tarArrColId
			int tarArrColId = static_cast<int>(absId) - cumulatedId;
			return std::make_pair(i, tarArrColId);
		}
		cumulatedId += curArrSize;
	}
	return std::make_pair(0,0);
}

std::pair<int,int> evalNextArrIds(int curArrId, int curLEDid, std::vector<int> ledCounts)
{
	if(curLEDid + 1 >= ledCounts[curArrId])
	{
		int nextArrId = (curArrId + 1) % ledCounts.size();
		return std::make_pair(curArrId, curLEDid + 1);
	}else{
		return std::make_pair(curArrId, curLEDid + 1);
	}
}

StaticLEDanimation::StaticLEDanimation(CRGB col)
	: _col(col),
	  _colChanged(true)
{
}

void StaticLEDanimation::update(std::vector<CRGB*>& strips, 
								std::vector<int>& ledCounts, 
								unsigned long dt)
{
	if(_colChanged)
	{
		for(auto stripId=0; stripId < strips.size(); ++stripId)
		{
			for(auto ledId=0; ledId < ledCounts[stripId]; ++ledId)
			{
				strips[stripId][ledId] = _col;
			}
		}
	}
	_colChanged = false;

	FastLED.show();
}


void StaticLEDanimation::setColor(CRGB col)
{
	_col = col;
	_colChanged = true;
}

//-------------------------------------------------------------

SmoothColorTransition::SmoothColorTransition(std::vector<CRGB> colors,
						  					 unsigned long duration)
	: _colors(colors),
	  _duration(duration)
{
}

void SmoothColorTransition::update(std::vector<CRGB*>& strips, 
								   std::vector<int>& ledCounts, 
								   unsigned long dt)
{	
	_progress += dt;
	if(_progress >= _duration)
	{
		_curColorId = (_curColorId + 1) % _colors.size();
		_progress = _progress % _duration;
	}

	CRGB curc = curColor();
	CRGB nxtc = nextColor();
	
	float progressPctg = static_cast<float>(_progress) / static_cast<float>(_duration);
	CRGB tarCol = composeCol(curc, nxtc, progressPctg);

	for(auto stripId=0; stripId < strips.size(); ++stripId)
	{
		for(auto ledId=0; ledId < ledCounts[stripId]; ++ledId)
		{
			strips[stripId][ledId] = tarCol;
		}
	}

	FastLED.show();
}

CRGB SmoothColorTransition::curColor() const
{
	return _colors[_curColorId];
}
CRGB SmoothColorTransition::nextColor() const
{
	int nextColId = (_curColorId + 1) % _colors.size();
	return _colors[nextColId];
}

void SmoothColorTransition::setColors(std::vector<CRGB> colors)
{
	_colors = colors;
	_curColorId = _curColorId % colors.size();
}
void SmoothColorTransition::setDuration(unsigned long duration)
{
	_duration = duration;
}



//------------------------------WaveLEDanimation------------------------------


WaveLEDanimation::WaveLEDanimation(std::vector<CRGB> waveColors,
								   CRGB peakColor,
							  	   std::vector<int> ledCounts,
								   unsigned long duration,
								   float range,
								   bool synchronze)
	: _waveColors(waveColors),
	  _peakColor(peakColor),
	  _ledCounts(ledCounts),
	  _synchronize(synchronze),
	  _duration(duration),
	  _range(range),
	  _totalLEDcount(0)
{
	evalTotalLEDcount(&_totalLEDcount, ledCounts);
}

long waveAnimSet = 0;
void WaveLEDanimation::update(std::vector<CRGB*>& strips, 
							  std::vector<int>& ledCounts, 
							  unsigned long dt)
{
	// if(waveAnimSet > 20)
	// 	return;
	// ++waveAnimSet;
	
	CRGB curCol = evalCurColor();

	if(_synchronize)
	{
		float firstIdFlt = evalFirstId();
		float rng = evalRange();
		float lastIdFlt = firstIdFlt + rng;

		// Serial.print("firstIdFlt: ");
		// Serial.print(firstIdFlt);
		// Serial.print("	rng: ");
		// Serial.print(rng);
		// Serial.print("	lastIdFlt: ");
		// Serial.print(lastIdFlt);
		// Serial.print("	curCol: ");
		// printColor(curCol);

		int firstId = static_cast<int>(firstIdFlt);

		// Serial.print("	firstId: ");
		// Serial.print(firstId);

		auto tarArrIds = evalArrId(firstIdFlt, _ledCounts);
		int curArrId 	= tarArrIds.first;
		int curArrLEDId = tarArrIds.second;

		// Serial.print("	curArrId: ");
		// Serial.print(curArrId);
		// Serial.print("	curArrLEDId: ");
		// Serial.print(curArrLEDId);
		auto nextLedArrIds = evalNextArrIds(curArrId, curArrLEDId, _ledCounts);
		CRGB lastCol = strips[nextLedArrIds.first][nextLedArrIds.second];

		for(int i=firstId; static_cast<float>(i) < lastIdFlt; ++i)
		{
			float i_flt = static_cast<float>(i);
			if( i_flt >= firstIdFlt ) // first i can be smaller than firstIdFlt
			{
				// CRGB lastCol = getLastColor();//strips[curArrId][curArrLEDId];

				float relProg = (i_flt - firstIdFlt) / (lastIdFlt - firstIdFlt);
				CRGB tarCol = composeCol(lastCol, curCol, relProg);

				// Serial.println();
				// Serial.print("	i_flt: ");
				// Serial.print(i_flt);
				// Serial.print("	curArrLEDId: ");
				// Serial.print(curArrLEDId);
				// Serial.print("	curArrId: ");
				// Serial.print(curArrId);
				// Serial.print("	relProg: ");
				// Serial.print(relProg);
				// Serial.print("	lastCol: ");
				// printColor(lastCol);
				// Serial.print("	tarCol: ");
				// printColor(tarCol);
				// Serial.print("	relProg: ");
				// Serial.print(relProg);

				strips[curArrId][curArrLEDId] = tarCol;
				
				curArrLEDId -= 1;
				if(curArrLEDId < 0)//>= ledCounts[curArrId])
				{
					curArrId = (curArrId - 1);
					if(curArrId < 0){
						curArrId = ledCounts.size() - 1;
					}
					curArrLEDId = ledCounts[curArrId] - 1;
					
				}
			}
		}
	}else{
		// for(int curArrId=0; curArrId < strips.size(); ++curArrId)
		// {
		// 	if(curArrId > 0 && ledCounts[0] == ledCounts[curArrId])
		// 	{
		// 		memmove( &strips[0][0], &strips[curArrId][0], ledCounts[0] * sizeof( CRGB) );
		// 	}else{
		// 		int curLedCount = ledCounts[curArrId];
				
		// 		float firstIdFlt = evalFirstId(curLedCount);
		// 		float rng = evalRange(curLedCount);
		// 		float lastIdFlt = firstIdFlt + rng;

		// 		int firstId = static_cast<int>(firstIdFlt);

		// 		int curArrColId = firstId;
				
		// 		for(int i=firstId; static_cast<float>(i) < lastIdFlt; ++i)
		// 		{
		// 			float i_flt = static_cast<float>(i);
		// 			if( i_flt >= firstIdFlt ) // first i can be smaller than firstIdFlt
		// 			{
		// 				CRGB lastCol = strips[curArrId][curArrColId];
		// 				float relProg = (i_flt - firstIdFlt) / (lastIdFlt - firstIdFlt);
		// 				CRGB tarCol = evalRangeColor(relProg, curCol, lastCol);

		// 				strips[curArrId][curArrColId] = tarCol;
						
		// 				curArrColId = (curArrColId + 1) % ledCounts[curArrId];
		// 			}
		// 		}
		// 	}
		// }
	}
	
	// Serial.print("\n	_progress bef: ");
	// Serial.print(_progress);
	_progress += static_cast<float>(dt) / static_cast<float>(_duration);
	if(_progress >= 1.0)
	{
		_curColId = (_curColId + 1) % _waveColors.size();
	}
	_progress = fmod(_progress, 1.0);
	
	// Serial.print("	_progress aft: ");
	// Serial.print(_progress);

	Serial.println();

	FastLED.show();
}


void WaveLEDanimation::setWaveColors(std::vector<CRGB> cols)
{
	_waveColors = cols;
	_curColId %= _waveColors.size();
}


CRGB WaveLEDanimation::getFirstColor()
{
	return _waveColors[_curColId];
}
CRGB WaveLEDanimation::getSecondColor()
{
	int scndColId = (_curColId + 1) % _waveColors.size();
	return _waveColors[scndColId];
}
CRGB WaveLEDanimation::getLastColor()
{
	int lastColId = _curColId == 0 ? _waveColors.size() - 1 : _curColId - 1;
	return _waveColors[lastColId];
}
CRGB WaveLEDanimation::evalCurColor()
{
	auto frstCol = getFirstColor();
	auto scndCol = getSecondColor();
	return composeCol(frstCol, scndCol, _progress);
}

float WaveLEDanimation::evalFirstId()
{
	return static_cast<float>(_totalLEDcount) * _progress;
}
float WaveLEDanimation::evalRange()
{
	return static_cast<float>(_totalLEDcount) * _range;
}


void WaveLEDanimation::setSynchronized(bool synchronize)
{
	_synchronize = synchronize;
}



//------------------------------WaveLEDanimation------------------------------


ShootingStarAnimation::ShootingStarAnimation(std::vector<CRGB> animCols,
									CRGB baseCol,
							  	   std::vector<int> ledCounts,
								   unsigned long duration,
								   float range,
								   bool synchronze)
	: _animCols(animCols),
	  _baseCol(baseCol),
	  _ledCounts(ledCounts),
	  _synchronize(synchronze),
	  _duration(duration),
	  _range(range),
	  _totalLEDcount(0)
{
	evalTotalLEDcount(&_totalLEDcount, ledCounts);
}

void ShootingStarAnimation::update(std::vector<CRGB*>& strips, 
							  std::vector<int>& ledCounts, 
							  unsigned long dt)
{
	auto tarCol = evalCurColor();

	if(_synchronize)
	{
		for(int i=0; i < ledCounts.size(); ++i){
			sinelon(tarCol, strips[i], ledCounts[i]);
		}
	}else{
	}
	
	_progress += static_cast<float>(dt) / static_cast<float>(_duration);
	if(_progress >= 1.0)
	{
		_curColId = (_curColId + 1) % _animCols.size();
	}
	_progress = fmod(_progress, 1.0);

	FastLED.show();
}

void ShootingStarAnimation::sinelon(CRGB tarCol, CRGB* strip, int ledCount)
{
  // a colored dot sweeping back and forth, with fading trails
  int rngInt = static_cast<int>(ledCount * _range);
  if(rngInt < 3){
	  rngInt = 3;
  }
  fadeToBlackBy( strip, ledCount, rngInt);
  int pos = beatsin16( 13, 0, ledCount-1 );
  strip[pos] = tarCol;
}

void ShootingStarAnimation::setAnimColors(std::vector<CRGB> cols)
{
	_animCols = cols;
}

CRGB ShootingStarAnimation::evalCurColor()
{
	auto col0 = getFirstColor();
	auto col1 = getSecondColor();
	return composeCol(col0, col1, _progress);
}

CRGB ShootingStarAnimation::getFirstColor()
{
	return _animCols[_curColId];
}
CRGB ShootingStarAnimation::getSecondColor()
{
	int scndColId = (_curColId + 1) % _animCols.size();
	return _animCols[scndColId];
}


void ShootingStarAnimation::setSynchronized(bool synchronize)
{
	_synchronize = synchronize;
}


