/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#include "MyGPS.h"

TinyGPSPlus tinyGps;

/* Constructor */
void MyGPS::init() {
	/* initialize object members with defaults */
	MYGPS_SERIAL.begin(9600);
}

bool MyGPS::update() {
	while (MYGPS_SERIAL.available() > 0)
		if (!tinyGps.encode(MYGPS_SERIAL.read())) {
			status = true;
		} else {
			status = false;
		}
	return status;
}

String MyGPS::getNMEA() {
	String receivedLine = MYGPS_SERIAL.readStringUntil('\n');
	DEBUG_MYGPS("RECIBO %s\n", receivedLine.c_str());
	return receivedLine;
}

String MyGPS::getTime() {
	String time;
	if (tinyGps.date.isValid() && tinyGps.time.isValid())
	{
		time = tinyGps.date.year();
		time.concat(F("-"));
		time.concat(tinyGps.date.month());
		time.concat(F("-"));
		time.concat(tinyGps.date.day());

		time.concat(F(" "));
		time.concat(tinyGps.time.hour());
		time.concat(F(":"));
		time.concat(tinyGps.time.minute());
		time.concat(F(":"));
		time.concat(tinyGps.time.second());
	}
	return time;
}

double MyGPS::getLat() {
	return tinyGps.location.lat();
}

double MyGPS::getLng() {
	return tinyGps.location.lng();
}

double MyGPS::getAlt() {
	return tinyGps.altitude.meters();
}

MyGPS gps;
