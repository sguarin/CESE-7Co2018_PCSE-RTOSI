#ifdef UNIT_TEST

#include <Arduino.h>
#include <MyGPS.h>
#include <unity.h>

#ifdef DEBUG_ESP_PORT
#define INFO_ESP_PORT DEBUG_ESP_PORT
#else
#define INFO_ESP_PORT Serial
#endif
#define INFO_TEST(...) INFO_ESP_PORT.print("INFO TEST: "); INFO_ESP_PORT.printf( __VA_ARGS__ )

void test_NMEA() {
	String nmea;
	nmea = gps.getNMEA();
	INFO_TEST("Recibi este NMEA: %s\n", nmea.c_str());
	//TEST_ASSERT_TRUE(.appendLine("1"));
}

void test_getLat() {
	// exact limits, as specified by EPSG:900913 / EPSG:3785 / OSGEO:41001
	double minLat = -85.05112878;
	double maxLat = 85.05112878;
	double Lat = gps.getLat();

	TEST_ASSERT(Lat > minLat && Lat < maxLat);
}

void test_getLng() {
	// exact limits, as specified by EPSG:900913 / EPSG:3785 / OSGEO:41001
	double minLng = -180;
	double maxLng = 180;
	double Lng = gps.getLng();

	TEST_ASSERT(Lng > minLng && Lng < maxLng);
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    gps.init();
}

uint8_t i = 0;
uint8_t count = 5;

void loop() {
	if (i < count) {
		INFO_TEST("Se chequea recibir una sentencia NMEA\n");
		RUN_TEST(test_NMEA);

		INFO_TEST("Se chequea recibir una Latitud\n");
		RUN_TEST(test_getLat);

		INFO_TEST("Se chequea recibir una Longitud\n");
		RUN_TEST(test_getLng);


		delay(500);
		i++;
	} else if (i == count) {
		UNITY_END(); // stop unit testing
	}
}

#endif
