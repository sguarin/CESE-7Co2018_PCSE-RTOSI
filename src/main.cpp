/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#include <Arduino.h>
#include <main.h>

typedef struct item {
	uint16_t CO2; // PPM
	uint16_t TVOC;

	unsigned long millis;
} dataItem_t;

#define DATA_STORE_QUEUE_SIZE		5
QueueHandle_t dataStoreQueue;
#define DATA_TRANSMIT_QUEUE_SIZE	10
QueueHandle_t dataTransmitQueue;

//! Task function to read data from sensors and push to Queues
/*!
	\param pvParameters its FreeRTOS arg
 */
void sensorsTask( void * pvParameters ) {
	BaseType_t rv;
	dataItem_t dataItem;
    while(true) {
    	if (Sensors.update()) {
    		// add data to queue
    		dataItem.CO2 = Sensors.getCO2();
    		dataItem.TVOC =  Sensors.getTVOC();
    		dataItem.millis =  Sensors.getRTC();
    		rv = xQueueSend(dataStoreQueue, &dataItem, portMAX_DELAY);
    		if (rv != pdPASS) {
    			DEBUG_MAIN("Error: pushing to dataStoreQueue\n");
    		}
    		rv = xQueueSend(dataTransmitQueue, &dataItem, portMAX_DELAY);
    		if (rv != pdPASS) {
    			DEBUG_MAIN("Error: pushing to dataTransmitQueue\n");
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
			DEBUG_MAIN("Error: getting data from dataStoreQueue\n");
		} else {
			dataLine = Sensors.getCO2();
			dataLine.concat(",");
			dataLine.concat(Sensors.getTVOC());
			dataLine.concat("\n");
			sd.appendLine(dataLine.c_str());
			DEBUG_MAIN("Dequeue sample CO2:%u\n", Sensors.getCO2());
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
	while (1) {
		rv = xQueueReceive(dataTransmitQueue, &dataItem, portMAX_DELAY);
		if (rv != pdPASS) {
			DEBUG_MAIN("Error: getting data from dataTransmitQueue\n");
		} else {
			// TODO
		}
	}
}

//! Arduino setup run once at start of main
void setup()
{
	// Serial initialization
	Serial.begin(115200);

	// I2C and sensors initialization
	Sensors.init();

	// SPI and SD initialization
	sd.init();

	// GPS initialization
	//gps.init();


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
		DEBUG_MAIN("Error creating queue\n");
	}

	dataTransmitQueue = xQueueCreate(DATA_TRANSMIT_QUEUE_SIZE, sizeof(dataItem_t));
	if (dataTransmitQueue == 0) {
		DEBUG_MAIN("Error creating queue\n");
	}

	DEBUG_MAIN("Setup initialization finished\n");
	delay(1000);
}

//! Arduino Loop hook. Whatchdog happens inside this task
void loop()
{
//	String nmea;
//	nmea = gps.getNMEA();
//	DEBUG_MAIN("RECIBO NMEA %s\n", nmea.c_str());
    delay(1000); //Don't spam the CPU
}


