#include "MySensors.h"
#include <SparkFunCCS811.h>
#include <Adafruit_BMP280.h>
#include <ClosedCube_HDC1080.h>

//#define CCS811_ADDR 0x5B //Default I2C Address
#define CCS811_ADDR 0x5A //Alternate I2C Address
CCS811 ccs811(CCS811_ADDR);
Adafruit_BMP280 bmp;
ClosedCube_HDC1080 hdc1080;

/* init */
bool MySensors::init() {

	bool ret = true;
	Wire.begin();
	//This setup routine is similar to what is used in the subclass' .begin() function
	CCS811Core::status returnCode = ccs811.begin(Wire); //Pass in the Wire port you want to use
	switch ( returnCode )
	{
	case CCS811Core::SENSOR_SUCCESS:
		DEBUG_MYSENSORS("CCS811 init success\n");
		break;
	case CCS811Core::SENSOR_ID_ERROR:
		DEBUG_MYSENSORS("CCS811 init error: ID_ERROR\n");
		ret &= 0;
		break;
	case CCS811Core::SENSOR_I2C_ERROR:
		DEBUG_MYSENSORS("CCS811 init error: I2C_ERROR\n");
		ret &= 0;
		break;
	case CCS811Core::SENSOR_INTERNAL_ERROR:
		DEBUG_MYSENSORS("CCS811 init error: INTERNAL_ERROR\n");
		ret &= 0;
		break;
	case CCS811Core::SENSOR_GENERIC_ERROR:
		DEBUG_MYSENSORS("CCS811 init error: GENERIC_ERROR\n");
		ret &= 0;
		break;
	default:
		DEBUG_MYSENSORS("CCS811 init unspecified error.\n");
		ret &= 0;
	}

	if(!bmp.begin(BMP280_ADDRESS_ALT)){
		DEBUG_MYSENSORS("BMP280 init failed!\n");
		ret &= 0;
	} else {
		DEBUG_MYSENSORS("BMP280 init success!\n");
	}

	hdc1080.begin(0x40);
	if (hdc1080.readManufacturerId() != 0x5449) {
		ret &= 0;
		DEBUG_MYSENSORS("HDC1080 init failed!\n");
	} else {
		DEBUG_MYSENSORS("HDC1080 init success!\n");
	}
	return ret;
}

bool MySensors::update() {
	//Check to see if data is ready with .dataAvailable()
	if (ccs811.dataAvailable())
	{
		//If so, have the sensor read and calculate the results.
		//Get them later
		ccs811.readAlgorithmResults();

		// request BMP280 measurement
		bmpPres = bmp.readPressure();
		bmpTemp = bmp.readTemperature();

		// read hdc measurement
		hdcTemp = hdc1080.readTemperature();
		hdcHum = hdc1080.readHumidity();

		// Set HUMID/TEMP for compensation
		ccs811.setEnvironmentalData(float(hdcHum), float(hdcTemp));

		DEBUG_MYSENSORS("TIME:%lu CO2:%u TVOC:%u  hdcTemp:%f hdcHum:%f bmpTemp:%f bmpPres:%f\n", millis(), ccs811.getCO2(), ccs811.getTVOC(), hdcTemp, hdcHum, bmpTemp, bmpPres);

		return true;
	}
	return false;
}

uint16_t MySensors::getCO2() {
	return ccs811.getCO2();
}

uint16_t MySensors::getTVOC() {
	return ccs811.getTVOC();
}

float MySensors::getTemp() {
	return float(hdcTemp);
}

float MySensors::getHum() {
	return float(hdcHum);
}

float MySensors::getPres() {
	return bmpPres;
}

MySensors sensors;

