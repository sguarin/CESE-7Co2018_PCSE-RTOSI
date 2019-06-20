#ifdef UNIT_TEST

#include <MySD.h>
#include <unity.h>

#ifdef DEBUG_ESP_PORT
#define INFO_ESP_PORT DEBUG_ESP_PORT
#else
#define INFO_ESP_PORT Serial
#endif
#define INFO_TEST(...) INFO_ESP_PORT.print("INFO TEST: "); INFO_ESP_PORT.printf( __VA_ARGS__ )

void test_create_new_file() {
	sd.remove();
	TEST_ASSERT_TRUE(sd.appendLine("1"));
}

void test_remove() {
	sd.appendLine("1");
	TEST_ASSERT_TRUE(sd.remove());
}

void test_remove_unexistent() {
	sd.remove();
	TEST_ASSERT_FALSE(sd.remove());
}

void test_hello_world() {
	const char *TEST_STRING = "Hello World";
	String tmp;

	sd.remove();
	TEST_ASSERT_TRUE(sd.appendLine(TEST_STRING));
	TEST_ASSERT_TRUE(sd.readLine(tmp));
	TEST_ASSERT_EQUAL_STRING(TEST_STRING, tmp.c_str());
}

void test_hello_world2() {
	const char *TEST_STRING = "Hello World\nSarasa\n";
	const char *TEST_STRING2 = "Hello World";
	String tmp;

	sd.remove();
	TEST_ASSERT_TRUE(sd.appendLine(TEST_STRING));
	TEST_ASSERT_TRUE(sd.readLine(tmp));
	TEST_ASSERT_EQUAL_STRING(TEST_STRING2, tmp.c_str());
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    sd.init();
}

uint8_t i = 0;
uint8_t count = 5;

void loop() {
	if (i < count) {
		INFO_TEST("Se crea un archivo nuevo\n");
		RUN_TEST(test_create_new_file);

		INFO_TEST("Se borra un archivo existente\n");
		RUN_TEST(test_remove);

		INFO_TEST("Se borra un archivo inexistente\n");
		RUN_TEST(test_remove_unexistent);

		INFO_TEST("Se crea un archivo con la cadena \"hello world\"\n");
		RUN_TEST(test_hello_world);

		INFO_TEST("Se crea un archivo con una cadena cuya primer linea es \"hello world\"\n");
		RUN_TEST(test_hello_world2);
		delay(500);
		i++;
	}
	else if (i == count) {
		UNITY_END(); // stop unit testing
	}
}

#endif
