#include "mySensors.h"
#include "SparkFunCCS811.h"
#include "BMP280.h"
#include "ClosedCube_HDC1080.h"

//#define CCS811_ADDR 0x5B //Default I2C Address
#define CCS811_ADDR 0x5A //Alternate I2C Address
CCS811 myGasSensor(CCS811_ADDR);

BMP280 bmp;


/* init */
void mySensors::init() {

	Wire.begin();
	//This setup routine is similar to what is used in the subclass' .begin() function
	CCS811Core::status returnCode = myGasSensor.begin(Wire); //Pass in the Wire port you want to use
	switch ( returnCode )
	{
	case CCS811Core::SENSOR_SUCCESS:
		DEBUG_MYSENSORS("SUCCESS\n");
		break;
	case CCS811Core::SENSOR_ID_ERROR:
		DEBUG_MYSENSORS("ID_ERROR\n");
		break;
	case CCS811Core::SENSOR_I2C_ERROR:
		DEBUG_MYSENSORS("I2C_ERROR\n");
		break;
	case CCS811Core::SENSOR_INTERNAL_ERROR:
		DEBUG_MYSENSORS("INTERNAL_ERROR\n");
		break;
	case CCS811Core::SENSOR_GENERIC_ERROR:
		DEBUG_MYSENSORS("GENERIC_ERROR\n");
		break;
	default:
		DEBUG_MYSENSORS("mySensors.init unspecified error.\n");
	}

	if(!bmp.begin()){
		DEBUG_MYSENSORS("BMP init failed!\n");
	} else
		DEBUG_MYSENSORS("BMP init success!\n");

	bmp.setOversampling(4);
}

bool mySensors::update() {


	//Check to see if data is ready with .dataAvailable()
	if (myGasSensor.dataAvailable())
	{

		//If so, have the sensor read and calculate the results.
		//Get them later
		myGasSensor.readAlgorithmResults();

		// Set HUMID/TEMP for compensation
		//myCCS811.setEnvironmentalData(BMEhumid, BMEtempC);

		DEBUG_MYSENSORS("TIME:%lu CO2:%u TVOC:%u\n", millis(), myGasSensor.getCO2(), myGasSensor.getTVOC());

		// request next BMP280 measurment
		char result = bmp.startMeasurment();
		result = bmp.getTemperatureAndPressure(bmpTemp, bmpPres);

		return true;
	}
	return false;
}

uint16_t mySensors::getCO2() {
	return myGasSensor.getCO2();
}

uint16_t mySensors::getTVOC() {
	return myGasSensor.getTVOC();
}

// TODO RTC
unsigned long mySensors::getRTC() {
	return millis();
}

mySensors Sensors;

