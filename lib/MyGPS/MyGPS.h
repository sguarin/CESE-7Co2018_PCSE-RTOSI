
/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef __MyGPS_h
#define __MyGPS_h

#include <Arduino.h>
#include <TinyGPS++.h>

#ifdef DEBUG
  #ifndef DEBUG_ESP_PORT
    #define DEBUG_ESP_PORT Serial
  #endif
  #ifndef DEBUG_ESP_WIFI
    #define DEBUG_ESP_WIFI 1
  #endif
  #define DEBUG_MYGPS(...) DEBUG_ESP_PORT.print("DEBUG MYGPS: "); DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
  #define DEBUG_MYGPS(...)
#endif

#define MYGPS_SERIAL Serial2

class MyGPS
{
  public:
    void init();
    bool update();

    String getNMEA();
    String getTime();
    double getLat();
    double getLng();
    double getAlt();

  private:
    bool status;

};

extern MyGPS gps;

#endif
