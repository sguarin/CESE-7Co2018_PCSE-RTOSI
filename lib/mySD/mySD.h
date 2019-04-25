/******************************************************************************

  Sebastián Guarino

  Distributed as-is; no warranty is given.
******************************************************************************/

#ifndef MYSD_H
#define MYSD_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#ifdef DEBUG
  #ifndef DEBUG_ESP_PORT
    #define DEBUG_ESP_PORT Serial
  #endif
  #ifndef DEBUG_ESP_WIFI
    #define DEBUG_ESP_WIFI 1
  #endif
  #define DEBUG_MYSD(...) DEBUG_ESP_PORT.print("DEBUG mySD: "); DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
  #define DEBUG_MYSD(...)
#endif

const char C_STATUS_EMPTY[] = "Vacia";
const char C_STATUS_PRESENT[] = "Presente";

const char C_MYSD_FILENAME[] PROGMEM = "/output.txt";

class mySD
{
  public:
    void init();
    void mount();
    String status;

    bool appendLine(const char *data);

    String getDir(const String &dirname);
    String getTotalBytes();
    String getUsedBytes();
    String getFreeBytes();


  private:
    String uint64ToString(uint64_t input);
    File outputFile;
};

extern mySD sd;

#endif
