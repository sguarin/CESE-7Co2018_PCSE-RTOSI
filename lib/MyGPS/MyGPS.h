
/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef MYGPS_H
#define MYGPS_H

#include <Arduino.h>

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

class myGPS
{
  public:
    void init();
    String status;

    String getNMEA();

  private:

};

extern myGPS gps;

#endif
