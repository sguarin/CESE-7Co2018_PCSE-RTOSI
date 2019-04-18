
#ifndef mySensors_h
#define mySensors_h

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

class mySensors
{
public:
	void init();
	bool update();

	//String status;
	//String getTotalBytes();
	uint16_t getCO2();
	uint16_t getTVOC();
	unsigned long getRTC();

protected:
	//Variables
	double bmpTemp,bmpPres;
};

extern mySensors Sensors;


#endif
