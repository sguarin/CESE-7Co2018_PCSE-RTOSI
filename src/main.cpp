/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/

#include <Arduino.h>
#include "main.h"

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
    		rv = xQueueSend(dataStoreQueue, &dataItem, 10/portTICK_PERIOD_MS);
    		if (rv != pdPASS) {
    			DEBUG_MAIN("Error: pushing to dataStoreQueue\n");
    		}
    		rv = xQueueSend(dataTransmitQueue, &dataItem, 10/portTICK_PERIOD_MS);
    		if (rv != pdPASS) {
    			DEBUG_MAIN("Error: pushing to dataStoreQueue\n");
    		}
    		// If I readed the sensors delay task for 1 second
    		vTaskDelay(1000 / portTICK_PERIOD_MS);
    	} else {
    		// If I didn't get sensor data delay task 50ms
    		vTaskDelay(50 / portTICK_PERIOD_MS);
    	}
    }
}

//! Task to dequeue data from dataQueue and write to SD
/*!
	\param pvParameters its FreeRTOS arg
 */
void SDTask( void * pvParameters) {
	BaseType_t rv;
	dataItem_t dataItem;
	while (1) {
		while (uxQueueMessagesWaiting(dataStoreQueue)) {
			rv = xQueueReceive(dataStoreQueue, &dataItem, 0);
			if (rv != pdPASS) {
				DEBUG_MAIN("Error: getting data from dataQueue\n");
			} else {

				sd.appendLine("")
				DEBUG_MAIN("Dequeue sample CO2:%u\n", Sensors.getCO2());
			}
		}
		vTaskDelay(2000 / portTICK_RATE_MS);
	}
}

void setup()
{
	Serial.begin(115200);

	Sensors.init();

	sd.init();

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

	dataStoreQueue = xQueueCreate(DATA_STORE_QUEUE_SIZE, sizeof(dataItem_t));
	if (dataStoreQueue == 0) {
		DEBUG_MAIN("Error creating queue\n");
	}

	dataTransmitQueue = xQueueCreate(DATA_TRANSMIT_QUEUE_SIZE, sizeof(dataItem_t));
	if (dataTransmitQueue == 0) {
		DEBUG_MAIN("Error creating queue\n");
	}
}

// Arduino Loop hook. Whatchdog happens inside this task
void loop()
{
    delay(1000); //Don't spam the I2C bus
}


