
/******************************************************************************

  Sebastián Guarino
  https://github.com/sguarin/CESE-7Co2018_PCSE-RTOSI.git

  Distributed as-is; no warranty is given.
******************************************************************************/
#include "MyMQTT.h"
#include <MQTT.h>
#include <WiFi.h>

WiFiClient net;
MQTTClient client;

/* init */
void MyMQTT::init(const char *host2, const char *brokerHost, const char *user2, const char *password2) {
	host = String(host2);
	user = String(user2);
	password = String(password2);
	client.begin(brokerHost, net);
}

bool MyMQTT::publish(const String &topic, const String &data) {
	if (!client.connected()) {
		  connect();
	}
	return client.publish(topic, data);
}

void MyMQTT::connect() {
	while (!client.connect(host.c_str(), user.c_str(), password.c_str())) {
		delay(1000);
	}
}

MyMQTT mqtt;

