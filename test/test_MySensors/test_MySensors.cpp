#ifdef UNIT_TEST

#include <Arduino.h>
#include <MySensors.h>
#include <unity.h>

#ifdef DEBUG_ESP_PORT
#define INFO_ESP_PORT DEBUG_ESP_PORT
#else
#define INFO_ESP_PORT Serial
#endif
#define INFO_TEST(...) INFO_ESP_PORT.print("INFO TEST: "); INFO_ESP_PORT.printf( __VA_ARGS__ )

void test_init() {
	TEST_ASSERT_TRUE(sensors.init());
}

void test_update() {
	bool ret;
	TEST_ASSERT_TRUE(sensors.init());

	// first update should return false
	TEST_ASSERT_FALSE(sensors.update());
	// wait for sensor to acquire a few seconds
	delay(4000);
	TEST_ASSERT_TRUE(sensors.update());
}


void test_getCO2() {
	uint16_t minCo2 = 100;
    uint16_t maxCo2 = 2000;
	uint16_t co2;

	test_update();

	co2 = sensors.getCO2();

	TEST_ASSERT(co2 > minCo2 && co2 < maxCo2);
}


void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

}

uint8_t i = 0;
uint8_t count = 5;

void loop() {
	if (i < count) {
		INFO_TEST("Se chequea iniciar la biblioteca (bus i2c y sensores)\n");
		RUN_TEST(test_init);

		INFO_TEST("Se chequea refrescar las mediciones\n");
		RUN_TEST(test_update);

		INFO_TEST("Se chequea recibir una concentración de CO2 en PPM normal\n");
		RUN_TEST(test_getCO2);

		delay(500);
		i++;
	} else if (i == count) {
		UNITY_END(); // stop unit testing
	}
}

#endif
