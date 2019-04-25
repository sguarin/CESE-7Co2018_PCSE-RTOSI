mqttServer="localhost"
mqttPort="1883"
channelSubs="co2"

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
 
# this is the syntax to follow for the elasticSearch update taken from documentation
#    es.index(index="my-index", doc_type="test-type", id=42, body={"any": +str(msg.payload, "timestamp": datetime.now()})
#    {u'_id': u'42', u'_index': u'my-index', u'_type': u'test-type', u'_version': 1, u'ok': True}

#our implementation uses this to separate numeric(float) from string data

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
	
    es.index(index="co2", doc_type="numeric", body={"topic" : msg.topic, "lat" : lat, "lng" : lng, "alt" : alt, "co2" : co2, "tvoc" : tvoc, "timestamp" : timestamp_obj, "timestamp_inserted" : datetime.utcnow()})
    #es.index(index="co2", doc_type="numeric", body={"topic" : msg.topic, "timestamp" : timestamp_obj, "lat" : lat, "lng" : lng, "alt" : alt, "co2" : co2, "tvoc" : tvoc, "timestamp_inserted": datetime.utcnow()})
	#es.index(index="co2", doc_type="numeric", body={"topic" : msg.topic, "dataFloat" : float(msg.payload), "timestamp": datetime.utcnow()})
    	
    #except:
	#es.index(index="co2", doc_type="string", body={"topic" : msg.topic, "dataString" : msg.payload, "timestamp": datetime.utcnow()})
    
# by default we connect to elasticSearch on localhost:9200
es = Elasticsearch()

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(mqttServer,mqttPort, 60)
#es.indices.delete(index='co2', ignore=400)
es.indices.create(index='co2', ignore=400)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()






