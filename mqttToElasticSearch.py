
# MQTT
mqttServer="localhost"
mqttPort="1883"
channelSubs="co2"

ES_INDEX="smmr"

#channelSubs="$SYS/#"
#use below as alternative to subscribe to all channels
#channelSubs="#"

import paho.mqtt.client as mqtt
from datetime import datetime
from elasticsearch import Elasticsearch


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe(channelSubs)

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))
 
    #try:
    a = msg.payload.split(',')
    timestamp_str = a[0]
    #timestamp_str = "2019-1-1 00:00:00"
    try:
      timestamp_obj = datetime.strptime(timestamp_str, '%Y-%m-%d %H:%M:%S')
    except:
      timestamp_obj = datetime.strptime("2000-01-01 00:00:00", '%Y-%m-%d %H:%M:%S')

    lat = a[1]
    lng = a[2]
    alt = a[3]
    co2 = float(a[4])
    tvoc = float(a[5])
    temp = float(a[6])
    hum = float(a[7])
    
    es.index(index=ES_INDEX, doc_type="_doc", body={"topic" : msg.topic, "location" : lat+","+lng, "alt" : alt, "co2" : co2, "tvoc" : tvoc, "temp" : temp, "hum" : hum, "timestamp" : timestamp_obj, "timestamp_inserted" : datetime.utcnow()})
    
# by default we connect to elasticSearch on localhost:9200
es = Elasticsearch()

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(mqttServer, mqttPort, 60)
#es.indices.delete(index=ES_INDEX, ignore=404)

request_body = {
    "mappings" : {
        "_doc" : {
            "properties" : {
                "location" : { "type" : "geo_point" },
                "timestamp" : { "type" : "date" }
            }
        }
    }
}

es.indices.create(index=ES_INDEX, ignore=400, body=request_body)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()

