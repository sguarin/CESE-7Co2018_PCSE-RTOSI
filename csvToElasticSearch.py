from datetime import datetime
from elasticsearch import Elasticsearch

def process_message(msg):
    print(str(msg))
    #try:
    a = msg.split(',')
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
	
    try:
        es.index(index="co2", doc_type="numeric", body={"topic" : "co2_gps", "lat" : lat, "lng" : lng, "alt" : alt, "co2" : co2, "tvoc" : tvoc, "timestamp" : timestamp_obj, "timestamp_inserted" : datetime.utcnow()})
    	
    except:
        print 'Error es.index'
	#es.index(index="co2", doc_type="string", body={"topic" : msg.topic, "dataString" : msg, "timestamp": datetime.utcnow()})
    
# by default we connect to elasticSearch on localhost:9200
es = Elasticsearch()
#es.indices.delete(index='co2', ignore=400)
es.indices.create(index='co2', ignore=400)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
# while read client.loop_forever()
for line in open('output_gps.txt'):
    process_message(line.rstrip())






