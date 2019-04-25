/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#include <Arduino.h>
#include <main.h>

typedef struct item {
	char time[20]; // DD/MM/AAAA HH:MM:SS
	unsigned long millis;

	double lat;
	double lng;
	double alt;

	uint16_t CO2; // PPM
	uint16_t TVOC;
} dataItem_t;

#define DATA_STORE_QUEUE_SIZE		5
QueueHandle_t dataStoreQueue;

#define DATA_TRANSMIT_QUEUE_SIZE	10
QueueHandle_t dataTransmitQueue;

xSemaphoreHandle serialLock;

#define SERIAL_MUTEX_LOCK()    do {} while (xSemaphoreTake(serialLock, portMAX_DELAY) != pdPASS)
#define SERIAL_MUTEX_UNLOCK()  xSemaphoreGive(serialLock)

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
#define INFO_MAIN(...) SERIAL_MUTEX_LOCK(); INFO_ESP_PORT.print("INFO MAIN: "); INFO_ESP_PORT.printf( __VA_ARGS__ ) ; SERIAL_MUTEX_UNLOCK()


/* Functions declarations */
String data2str(dataItem_t *dataItem);
void sensorsTask( void * pvParameters );
void SDTask( void * pvParameters);
void transmitTask (void *pvParameters);
/* End Functions declarations */


//! Task function to read data from sensors and push to Queues
/*!
	\param pvParameters its FreeRTOS arg
 */
void sensorsTask( void * pvParameters ) {
	BaseType_t rv;
	dataItem_t dataItem;
    while(true) {
    	if (Sensors.update() && gps.update()) {
    		// add data to queue
    		dataItem.CO2 = Sensors.getCO2();
    		dataItem.TVOC =  Sensors.getTVOC();
    		dataItem.millis =  Sensors.getRTC();

    		strncpy(dataItem.time, gps.getTime().c_str(), 20);
    		dataItem.lat = gps.getLat();
    		dataItem.lng = gps.getLng();
    		dataItem.alt = gps.getAlt();

    		rv = xQueueSend(dataStoreQueue, &dataItem, portMAX_DELAY);
    		if (rv != pdPASS) {
    			INFO_MAIN("Error: pushing to dataStoreQueue\n");
    		}
    		rv = xQueueSend(dataTransmitQueue, &dataItem, portMAX_DELAY);
    		if (rv != pdPASS) {
    			INFO_MAIN("Error: pushing to dataTransmitQueue\n");
    		}
    		// If I readed the sensors delay task for 1 second
    		vTaskDelay(1000 / portTICK_PERIOD_MS);
    	} else {
    		// If I didn't get sensor data delay task 50ms
    		vTaskDelay(50 / portTICK_PERIOD_MS);
    	}
    }
}

//! Task to dequeue data from dataStoreQueue and write to SD
/*!
	\param pvParameters its FreeRTOS arg
 */
void SDTask( void * pvParameters) {
	BaseType_t rv;
	dataItem_t dataItem;
	String dataLine;
	while (1) {
		rv = xQueueReceive(dataStoreQueue, &dataItem, portMAX_DELAY);
		if (rv != pdPASS) {
			INFO_MAIN("Error: getting data from dataStoreQueue\n");
		} else {
			dataLine = data2str(&dataItem);
			dataLine.concat('\n');
			sd.appendLine(dataLine.c_str());
			DEBUG_MAIN("Saving sample:%s", dataLine.c_str());
		}
	}
}

//! Task to dequeue data from dataTransmitQueue and transmit
/*!
	\param pvParameters its FreeRTOS arg
 */
void transmitTask (void *pvParameters) {
	BaseType_t rv;
	dataItem_t dataItem;
	String dataLine;
	while (1) {
		rv = xQueueReceive(dataTransmitQueue, &dataItem, portMAX_DELAY);
		if (rv != pdPASS) {
			INFO_MAIN("Error: getting data from dataTransmitQueue\n");
		} else {
			dataLine = data2str(&dataItem);
			if (WiFi.status() == WL_CONNECTED)
				mqtt.publish("co2", dataLine);
		}
	}
}

String data2str(dataItem_t *dataItem) {
	String dataLine;
	dataLine = (const char *)dataItem->time;
	dataLine.concat(",");
	dataLine.concat(dataItem->lat);
	dataLine.concat(",");
	dataLine.concat(dataItem->lng);
	dataLine.concat(",");
	dataLine.concat(dataItem->alt);
	dataLine.concat(",");
	dataLine.concat(dataItem->CO2);
	dataLine.concat(",");
	dataLine.concat(dataItem->TVOC);
	return dataLine;
}

//! Arduino setup run once at start of main
void setup()
{
	// Serial initialization
	Serial.begin(115200);

	// Wifi initilization
	uint8_t n = 0;
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED && n <= 10) {
		Serial.print(".");
		n++;
		delay(1000);
	}

	// I2C and sensors initialization
	Sensors.init();

	// SPI and SD initialization
	sd.init();

	// MQTT Client initialization
	mqtt.init(HOST, MQTT_HOST, MQTT_USER, MQTT_PASSWORD);

	// GPS initialization
	gps.init();

	xTaskCreatePinnedToCore(
			sensorsTask,   /* Function to implement the task */
			"sensorsTask", /* Name of the task */
			10000,      /* Stack size in words */
			NULL,       /* Task input parameter */
			1,          /* Priority of the task */
			NULL,       /* Task handle. */
			1);  /* Core where the task should run */

	xTaskCreatePinnedToCore(
			SDTask,   /* Function to implement the task */
			"SDTask", /* Name of the task */
			10000,      /* Stack size in words */
			NULL,       /* Task input parameter */
			1,          /* Priority of the task */
			NULL,       /* Task handle. */
			1);  /* Core where the task should run */

	xTaskCreatePinnedToCore(
			transmitTask,   /* Function to implement the task */
			"transmitTask", /* Name of the task */
			10000,      /* Stack size in words */
			NULL,       /* Task input parameter */
			1,          /* Priority of the task */
			NULL,       /* Task handle. */
			1);  /* Core where the task should run */

	dataStoreQueue = xQueueCreate(DATA_STORE_QUEUE_SIZE, sizeof(dataItem_t));
	if (dataStoreQueue == 0) {
		INFO_MAIN("Error creating queue\n");
	}

	dataTransmitQueue = xQueueCreate(DATA_TRANSMIT_QUEUE_SIZE, sizeof(dataItem_t));
	if (dataTransmitQueue == 0) {
		INFO_MAIN("Error creating queue\n");
	}

	INFO_MAIN("Setup initialization finished\n");
	delay(1000);
}

//! Arduino Loop hook. Whatchdog happens inside this task
void loop()
{
    delay(1000); //Don't spam the CPU
}


