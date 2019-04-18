
#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <WiFi.h>
#ifdef ESP32
#include <WebServer.h>
#else
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#endif

#include <mySD.h>
#include <mySensors.h>


#ifdef DEBUG
  #ifndef DEBUG_ESP_PORT
    #define DEBUG_ESP_PORT Serial
  #endif
  #ifndef DEBUG_ESP_WIFI
    #define DEBUG_ESP_WIFI 1
  #endif
  #define DEBUG_MAIN(...) DEBUG_ESP_PORT.print("DEBUG MAIN: "); DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#else
  #define DEBUG_MAIN(...)
#endif

#ifdef DEBUG_ESP_PORT
#define INFO_ESP_PORT DEBUG_ESP_PORT
#else
#define INFO_ESP_PORT Serial
#endif
#define INFO_MAIN(...) INFO_ESP_PORT.print("INFO MAIN: "); INFO_ESP_PORT.printf( __VA_ARGS__ )

#define HOST "sadpefe"
#define ADMIN_USERNAME "admin"
#define ADMIN_PASSWORD "sadpefe"

/* ROM Stored strings */
const char GC_TEXT_HTML[] PROGMEM = "text/html";

#endif


