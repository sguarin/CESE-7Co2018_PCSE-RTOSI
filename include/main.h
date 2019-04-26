
#ifndef __MAIN_H
#define __MAIN_H

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
#include <MyMQTT.h>
#include <MyGPS.h>

#define WIFI_SSID "NONAME 2.4GHz.-"
#define WIFI_PASSWORD "mafra00000"

//#define WIFI_SSID "mihu"
//#define WIFI_PASSWORD "mihu00000"

#define HOST "sadpefe"
#define MQTT_HOST "mine.sguarin.com.ar"
#define MQTT_USER "sadpefe_client"
#define MQTT_PASSWORD "sadpefe_client00"

/* ROM Stored strings */
const char GC_HOST[] PROGMEM = HOST;

#endif


