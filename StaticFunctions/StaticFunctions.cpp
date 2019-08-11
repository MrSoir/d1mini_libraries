#include "StaticFunctions.h"


namespace SF
{

	std::vector<String> splitString(String str, char sep)
	{
		std::vector<String> ss;
		auto lastChrId = 0;
		auto chrId = str.indexOf(sep, lastChrId);
		while(chrId > -1)
		{
		auto substr = str.substring(lastChrId, chrId);
		ss.push_back(substr);

		lastChrId = chrId + 1;
		chrId = str.indexOf(sep, lastChrId);
		}
		if(lastChrId < str.length())
		{
		ss.push_back( str.substring(lastChrId) );
		}
		return ss;
	}

	int evalDayOfWeek(unsigned long unix_time, int UNIX_DAY_OFFSET)
	{
		return ((unix_time / 86400) + UNIX_DAY_OFFSET) % 7;
	}
	int evalCurrentDayOfWeek(unsigned long UNIX_TIME, int UNIX_DAY_OFFSET)
	{
		return evalDayOfWeek(UNIX_TIME, UNIX_DAY_OFFSET);
	}

	int evalDayTime(unsigned long unix_time)
	{
		return unix_time % 86400;
	}
	int evalCurrentDayTime(unsigned long UNIX_TIME)
	{
		return evalDayTime(UNIX_TIME);
	}

	int evalDayEndTime(int startDayTime, int duration)
	{
		return (startDayTime + duration) % 86400;
	}
	int powInt(int base, int exponent)
	{
		float basef = static_cast<float>(base);
		float exponentf = static_cast<float>(exponent);
		return static_cast<int>( pow(basef, exponentf) );
	}

	std::vector<bool> daysOfWeekIntToBoolArr(int powerVal)
	{
		std::vector<bool> dow(7, false);
		for(int i=6; i >= 0; --i)
		{
		int power = powInt(2, static_cast<int>(i));
		dow[i] = powerVal >= power;
		powerVal %= power;
		}
		return dow;
	}

	int arrayToDaysWofWeek(JsonArray arr)
	{
		int sum = 0;
		for(int i=0; i < arr.size(); ++i)
		{
		sum += static_cast<int>(std::pow(2, i));
		}
		return sum;
	}

	std::tuple<DynamicJsonDocument, int> serverGET(String url, const size_t bufferSize, const uint16_t timeout)
	{
		DynamicJsonDocument jsonDoc(bufferSize);
		int retValidationCode = 0;
		
		if (WiFi.status() == WL_CONNECTED) {
			HTTPClient http;
	//		http.setTimeout(timeout);

			http.begin( url );
			int httpCode = http.GET();
			if(httpCode > 0)
			{
				auto http_str = http.getString();
				deserializeJson(jsonDoc, http_str);

				retValidationCode = 1;
			}else{
				Serial.print("http.GET() == 0 -> error requesting '");
				Serial.print(url);
				Serial.println("'!");
			}
			http.end();
		}else{
			Serial.print("could not request serverGET '");
			Serial.print(url);
			Serial.println("' -> no WiFi connection!");
		}
		return std::make_tuple(jsonDoc, retValidationCode);
	}
	
	String genServerRequestUrl(String urlRelPath, const String& SERVER_MDNS, const String& SERVER_PORT, const String& ARDUINO_ID){
		int lastSlashId = urlRelPath.lastIndexOf('/');
		
		String lastUrlPathFrgmnt = (lastSlashId > -1) ? urlRelPath.substring(lastSlashId) : urlRelPath;
		bool alrContainsArgs = lastUrlPathFrgmnt.indexOf('?') > -1;
		
		String append = (alrContainsArgs ? String('&') : String('?')) + String("arduinoId=") + String(ARDUINO_ID);
		
		return "http://" + SERVER_MDNS + ":" + SERVER_PORT + urlRelPath + append;
	}

	//------------------------------------------------------------------------------

	unsigned long hexStringToULong(const std::string& hex_str){
		unsigned long hex_value = std::strtoul(hex_str.c_str(), 0, 16);
		return hex_value;
	}

	std::tuple<unsigned long,unsigned long,unsigned long> hexColorStringToULongs(const std::string& hexColStr)
	{
		int id = hexColStr[0] == '#' ? 1 : 0;
		
		auto cs0 = hexColStr.substr(id,   2);
		auto cs1 = hexColStr.substr(id+2, 2);
		auto cs2 = hexColStr.substr(id+4, 2);
		
		auto hex0 = hexStringToULong(cs0);
		auto hex1 = hexStringToULong(cs1);
		auto hex2 = hexStringToULong(cs2);
		
		return std::make_tuple(hex0, hex1, hex2);
	}

	std::tuple<unsigned long,unsigned long,unsigned long> hexColorStringToULongs(const String& hexColStr)
	{
		std::string stdStr = hexColStr.c_str();
		return hexColorStringToULongs(stdStr);
	}
}


