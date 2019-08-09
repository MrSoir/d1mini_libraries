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
	
	Serial.println();
	Serial.print("col0: ");
	printColor(col0);
	Serial.print("	col1: ");
	printColor(col1);
	Serial.print("	perctg: ");
	Serial.print(perctg);
	Serial.print("	dr: ");
	Serial.print(dr);
	Serial.print("	dg: ");
	Serial.print(dg);
	Serial.print("	db: ");
	Serial.print(db);
	Serial.print("	tarCol: ");
	printColor(tarCol);
	Serial.println();
	
	return tarCol;
}


//------------------------------StaticLEDanimation------------------------------

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
	Serial.print("SmoothColorTransition::Constructor: colors.size: ");
	Serial.print(colors.size());
	Serial.print("	duration: ");
	Serial.println(duration);

	for(int i=0; i < _colors.size(); ++i)
	{
		Serial.print("col[");
		Serial.print(i);
		Serial.print("]: ");
		printColor(_colors[i]);
		Serial.print("	");
	}
	Serial.println();
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

	Serial.print("SmoothColorTransition:: urc: ");
	printColor(curc);
	Serial.print("	nxtc: ");
	printColor(nxtc);
	
	float progressPctg = static_cast<float>(_progress) / static_cast<float>(_duration);
	CRGB tarCol = composeCol(curc, nxtc, progressPctg);

	Serial.print("	_progress: ");
	Serial.print(_progress);
	Serial.print("	rogressPctg: ");
	Serial.print(progressPctg);
	Serial.print("	_curColorId: ");
	Serial.print(_curColorId);

	Serial.print("	tarCol: ");
	printColor(tarCol);
	Serial.println();

	for(auto stripId=0; stripId < strips.size(); ++stripId)
	{
		for(auto ledId=0; ledId < ledCounts[stripId]; ++ledId)
		{
			strips[stripId][ledId] = tarCol;
		}
	}
	
/*	for(int curArrId=0; curArrId < strips.size(); ++curArrId)
	{
		if(curArrId > 0 && ledCounts[curArrId] == ledCounts[0])
		{
			memmove( &strips[0][0], &strips[curArrId][0], ledCounts[0] * sizeof( CRGB) );
		}else{
			for(int ledId=0; ledId < ledCounts[curArrId]; ++ledId)
			{
				strips[curArrId][ledId] = tarCol;
			}
		}
	}
*/

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
								   float baseBrightness,
							  	   std::vector<int> ledCounts,
								   unsigned long duration,
								   float range,
								   bool synchronze)
	: _waveColors(waveColors),
	  _baseBrightness(baseBrightness),
	  _ledCounts(ledCounts),
	  _synchronize(synchronze),
	  _duration(duration),
	  _range(range),
	  _totalLEDcount(0)
{
	evalTotalLEDcount();
}

void WaveLEDanimation::update(std::vector<CRGB*>& strips, 
							  std::vector<int>& ledCounts, 
							  unsigned long dt)
{
	CRGB curCol = evalCurColor();

	if(_synchronize)
	{
		float firstIdFlt = evalFirstId(_totalLEDcount);
		float rng = evalRange(_totalLEDcount);
		float lastIdFlt = firstIdFlt + rng;

		int firstId = static_cast<int>(firstIdFlt);

		auto tarArrIds = evalArrId(firstIdFlt);
		int curArrId 	= tarArrIds.first;
		int curArrColId = tarArrIds.second;
		
		for(int i=firstId; static_cast<float>(i) < lastIdFlt; ++i)
		{
			float i_flt = static_cast<float>(i);
			if( i_flt >= firstIdFlt ) // first i can be smaller than firstIdFlt
			{
				CRGB lastCol = strips[curArrId][curArrColId];

				float relProg = (i_flt - firstIdFlt) / (lastIdFlt - firstIdFlt);
				CRGB tarCol = evalRangeColor(relProg, curCol, lastCol);

				strips[curArrId][curArrColId] = tarCol;
				
				curArrColId += 1;
				if(curArrColId >= ledCounts[curArrId])
				{
					curArrColId = 0;
					curArrId = (curArrId + 1) % ledCounts.size();
				}
			}
		}
	}else{
		for(int curArrId=0; curArrId < strips.size(); ++curArrId)
		{
			if(curArrId > 0 && ledCounts[0] == ledCounts[curArrId])
			{
				memmove( &strips[0][0], &strips[curArrId][0], ledCounts[0] * sizeof( CRGB) );
			}else{
				int curLedCount = ledCounts[curArrId];
				
				float firstIdFlt = evalFirstId(curLedCount);
				float rng = evalRange(curLedCount);
				float lastIdFlt = firstIdFlt + rng;

				int firstId = static_cast<int>(firstIdFlt);

				int curArrColId = firstId;
				
				for(int i=firstId; static_cast<float>(i) < lastIdFlt; ++i)
				{
					float i_flt = static_cast<float>(i);
					if( i_flt >= firstIdFlt ) // first i can be smaller than firstIdFlt
					{
						CRGB lastCol = strips[curArrId][curArrColId];
						float relProg = (i_flt - firstIdFlt) / (lastIdFlt - firstIdFlt);
						CRGB tarCol = evalRangeColor(relProg, curCol, lastCol);

						strips[curArrId][curArrColId] = tarCol;
						
						curArrColId = (curArrColId + 1) % ledCounts[curArrId];
					}
				}
			}
		}
	}
	
	_progress += static_cast<float>(dt) / static_cast<float>(_duration);
	_progress = fmod(_progress, 1.0);
}

void WaveLEDanimation::setWaveColors(std::vector<CRGB> cols)
{
	_waveColors = cols;
	_curColId %= _waveColors.size();
}

std::pair<int,int> WaveLEDanimation::evalArrId(float absId)
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

CRGB WaveLEDanimation::getFirstColor()
{
	return _waveColors[_curColId];
}
CRGB WaveLEDanimation::getSecondColor()
{
	int scndColId = (_curColId + 1) % _waveColors.size();
	return _waveColors[scndColId];
}
CRGB WaveLEDanimation::evalCurColor()
{
	auto frstCol = getFirstColor();
	auto scndCol = getSecondColor();
	return composeCol(frstCol, scndCol, _progress);//frstCol + (scndCol - frstCol) * _progress;
}
CRGB WaveLEDanimation::evalRangeColor(float relProg, CRGB rngCol, CRGB lastCol)
{
	if(relProg > 0.5)
	{
		float mappedProg = (relProg - 0.5) * 2.0;
		CRGB dimmedCol = CRGB(static_cast<uint8_t>(static_cast<float>(rngCol.r) * _baseBrightness), 
							  static_cast<uint8_t>(static_cast<float>(rngCol.r) * _baseBrightness), 
							  static_cast<uint8_t>(static_cast<float>(rngCol.r) * _baseBrightness));
		
		return composeCol(rngCol, dimmedCol, mappedProg);//rngCol + (dimmedCol - rngCol) * mappedProg;
	}else{
		float mappedProg = relProg * 2.0;
		CRGB dimmedCol = lastCol;

		return composeCol(lastCol, rngCol, mappedProg);//lastCol + (rngCol - lastCol) * mappedProg;
	}
}

float WaveLEDanimation::evalFirstId(int ledsCnt)
{
	return static_cast<float>(ledsCnt) * _progress;
}
float WaveLEDanimation::evalRange(int ledsCnt)
{
	return static_cast<float>(ledsCnt) * _range;
}


void WaveLEDanimation::setSynchronized(bool synchronize)
{
	_synchronize = synchronize;
}

void WaveLEDanimation::evalTotalLEDcount()
{
	if(_synchronize)
	{
		_totalLEDcount = 0;
		for(int i=0; i < _ledCounts.size(); ++i)
		{
			_totalLEDcount += _ledCounts[i];
		}
	}
}
