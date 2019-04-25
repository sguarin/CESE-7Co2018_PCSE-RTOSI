
/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef myMQTT_h
#define myMQTT_h

#include <Arduino.h>

#ifdef DEBUG
  #ifndef DEBUG_ESP_PORT
    #define DEBUG_ESP_PORT Serial
  #endif
  #ifndef DEBUG_ESP_WIFI
    #define DEBUG_ESP_WIFI 1
  #endif
  #define DEBUG_MYMQTT(...) DEBUG_ESP_PORT.print("DEBUG myMQTT: "); DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
  #define DEBUG_MYMQTT(...)
#endif

class myMQTT
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

extern myMQTT mqtt;

#endif
