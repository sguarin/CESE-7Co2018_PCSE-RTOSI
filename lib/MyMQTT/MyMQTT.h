
/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef __MyMQTT_h
#define __MyMQTT_h

#include <Arduino.h>

#ifdef DEBUG
  #ifndef DEBUG_ESP_PORT
    #define DEBUG_ESP_PORT Serial
  #endif
  #ifndef DEBUG_ESP_WIFI
    #define DEBUG_ESP_WIFI 1
  #endif
  #define DEBUG_MYMQTT(...) DEBUG_ESP_PORT.print("DEBUG MyMQTT: "); DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
  #define DEBUG_MYMQTT(...)
#endif

class MyMQTT
{
public:
	void init(const char *host, const char *brokerHost, const char *user, const char *password);
	bool publish(const String &topic, const String &data);

protected:
	void connect();

	//Variables
	String host;
	String user;
	String password;

};

extern MyMQTT mqtt;

#endif
