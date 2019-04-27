
#ifndef __MySensors_h__
#define __MySensors_h__

#include <Arduino.h>
#include <Wire.h>

#ifdef DEBUG
  #ifndef DEBUG_ESP_PORT
    #define DEBUG_ESP_PORT Serial
  #endif
  #ifndef DEBUG_ESP_WIFI
    #define DEBUG_ESP_WIFI 1
  #endif
  #define DEBUG_MYSENSORS(...) DEBUG_ESP_PORT.print("DEBUG mySensors: "); DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
  #define DEBUG_MYSENSORS(...)
#endif

class MySensors
{
public:
	bool init();
	bool update();

	uint16_t getCO2();
	uint16_t getTVOC();
	float getTemp();
	float getHum();
	float getPres();

protected:
	//Variables
	float bmpTemp;
	float bmpPres;
	double hdcTemp;
	double hdcHum;
};

extern MySensors sensors;

#endif
