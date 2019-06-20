/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#include <Arduino.h>
#include <main.h>

/*==================[macros and definitions]=================================*/

#define DATA_STORE_QUEUE_SIZE		5
#define DATA_TRANSMIT_QUEUE_SIZE	10

#define SERIAL_MUTEX_LOCK()    do {} while (xSemaphoreTake(serialLock, portMAX_DELAY) != pdPASS)
#define SERIAL_MUTEX_UNLOCK()  xSemaphoreGive(serialLock)

#ifdef DEBUG
  #ifndef DEBUG_ESP_PORT
    #define DEBUG_ESP_PORT Serial
  #endif
  #ifndef DEBUG_ESP_WIFI
    #define DEBUG_ESP_WIFI 1
  #endif
  #define DEBUG_MAIN(...) SERIAL_MUTEX_LOCK(); DEBUG_ESP_PORT.print("DEBUG MAIN: "); DEBUG_ESP_PORT.printf( __VA_ARGS__ ); SERIAL_MUTEX_UNLOCK()
#else
  #define DEBUG_MAIN(...)
#endif

#ifdef DEBUG_ESP_PORT
#define INFO_ESP_PORT DEBUG_ESP_PORT
#else
#define INFO_ESP_PORT Serial
#endif
#define INFO_MAIN(...) SERIAL_MUTEX_LOCK(); INFO_ESP_PORT.print("INFO MAIN: "); INFO_ESP_PORT.printf( __VA_ARGS__ ) ; SERIAL_MUTEX_UNLOCK()

/*==================[internal data definition]===============================*/
typedef struct item {
	char time[20]; // DD/MM/AAAA HH:MM:SS

	double lat;
	double lng;
	double alt;

	uint16_t co2; // PPM
	uint16_t tvoc; // PPM
	float temp;
	float pres;
	float hum;
} dataItem_t;

QueueHandle_t dataStoreQueue;
QueueHandle_t dataTransmitQueue;
SemaphoreHandle_t serialLock;

/*==================[internal functions declaration]=========================*/
String data2str(dataItem_t *dataItem);
void sensorsTask( void * pvParameters );
void SDTask( void * pvParameters);
void transmitTask (void *pvParameters);
void loopForever();


/*==================[internal functions definition]==========================*/

//! Task function to read data from sensors and push to Queues
/*!
	\param pvParameters its FreeRTOS arg
 */
void sensorsTask( void * pvParameters ) {
	BaseType_t rv;
	dataItem_t dataItem;
    while(true) {
    	if (sensors.update() && gps.update()) {
    		// add data to queue
    		dataItem.co2 = sensors.getCO2();
    		dataItem.tvoc =  sensors.getTVOC();

    		dataItem.temp = sensors.getTemp();
    		dataItem.hum = sensors.getHum();
    		dataItem.pres = sensors.getPres();

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
	char geoPoint[30];
	dataLine = (const char *)dataItem->time;
	dataLine.concat(",");
	// TODO BUG Report upstream Arduino String concat double
	snprintf(geoPoint, 29, "%f,%f", dataItem->lat, dataItem->lng);
	dataLine.concat(geoPoint);
//	dataLine.concat(dataItem->lat);
//	dataLine.concat(",");
//	dataLine.concat(dataItem->lng);
	dataLine.concat(",");
	dataLine.concat(dataItem->alt);
	dataLine.concat(",");
	dataLine.concat(dataItem->co2);
	dataLine.concat(",");
	dataLine.concat(dataItem->tvoc);
	dataLine.concat(",");
	dataLine.concat(dataItem->temp);
	dataLine.concat(",");
	dataLine.concat(dataItem->hum);

	return dataLine;
}

//! Loop Forever
void loopForever()
{
    delay(1000); //Don't spam the CPU
}

/*==================[external functions definition]==========================*/
#ifndef UNIT_TEST
//! Arduino setup run once at start of main
void setup()
{
	// Serial initialization
	Serial.begin(115200);

	// create serial MUTEX
	serialLock = xSemaphoreCreateMutex();
	if (serialLock == 0) {
		INFO_ESP_PORT.print("INFO MAIN: Error creating serial MUTEX\n");
		//INFO_MAIN("Error creating serial MUTEX\n");
	}

	// Wifi initilization
	uint8_t n = 0;
	WiFi.setAutoConnect(false);
	WiFi.setAutoReconnect(false);
	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
	while (WiFi.status() != WL_CONNECTED && n <= 10) {
		Serial.print(".");
		n++;
		delay(1000);
	}
	if (WiFi.status() != WL_CONNECTED) {
		WiFi.disconnect();
		INFO_MAIN("WIFI Disabled\n");
		delay(1000);
	}

	// I2C and sensors initialization
	if (!sensors.init()) {
		DEBUG_MAIN("Error inicializando Sensores");
		loopForever();
	}

	// SPI and SD initialization
	sd.init();

	// MQTT Client initialization
	if (WiFi.status() == WL_CONNECTED)
		mqtt.init(HOST, MQTT_HOST, MQTT_USER, MQTT_PASSWORD);

	// GPS initialization
	gps.init();

	// create queue for data to store in sd
	dataStoreQueue = xQueueCreate(DATA_STORE_QUEUE_SIZE, sizeof(dataItem_t));
	if (dataStoreQueue == 0) {
		INFO_MAIN("Error creating store queue\n");
	}

	// create queue for data to transmit via mqtt
	dataTransmitQueue = xQueueCreate(DATA_TRANSMIT_QUEUE_SIZE, sizeof(dataItem_t));
	if (dataTransmitQueue == 0) {
		INFO_MAIN("Error creating transmit queue\n");
	}

	// create tasks
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

	INFO_MAIN("Setup initialization finished\n");
	delay(1000);
}

//! Arduino Loop hook. Whatchdog happens inside this task
void loop()
{
    delay(1000); //Don't spam the CPU
}

#endif // UNIT_TEST

