/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#include "myGPS.h"

/* Constructor */
void myGPS::init() {
	/* initialize object members with defaults */
	Serial1.begin(9600, SERIAL_8N1);

}

String myGPS::getNMEA() {
	String receivedLine = Serial1.readStringUntil('\n');
	DEBUG_MYGPS("RECIBO %s\n", receivedLine.c_str());
	return receivedLine;
}

myGPS gps;
