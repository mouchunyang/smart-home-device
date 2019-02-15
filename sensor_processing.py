"""EE 250L Lab 11 Final Project

sensor_processing.py: Sensor data processing.

TODO: List team members here.

TODO: Insert Github repository link here.

"""
import paho.mqtt.client as mqtt
import time
import requests
import json
from datetime import datetime

# MQTT variables
broker_hostname = "eclipse.usc.edu"
broker_port = 11000

#uncomment these lines to subscribe to real-time published data
topic = "moucai/light"


#uncomment these lines to subscribe to recorded data being played back in a loop
# ultrasonic_ranger1_topic = "ultrasonic_ranger1/fake_data"
# ultrasonic_ranger2_topic = "ultrasonic_ranger2/fake_data"

# Lists holding the ultrasonic ranger sensor distance readings. Change the 
# value of MAX_LIST_LENGTH depending on how many distance samples you would 
# like to keep at any point in time.

#determine the current state
currentstate="start"
changed=0

def light_callback(client, userdata, msg):
    #print("msg.payload is"+str(msg.payload))
    global currentstate
    global changed
    received=str(msg.payload.decode('utf-8'))
    #print(received)
    if(received!=currentstate):
        currentstate=received
        print("currentstate is: "+str(currentstate))
        #print("statechanged")
        changed=1

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe(topic)
    client.message_callback_add(topic, light_callback)


# The callback for when a PUBLISH message is received from the server.
# This should not be called.
def on_message(client, userdata, msg): 
    print(msg.topic + " " + str(msg.payload))

if __name__ == '__main__':
    # Connect to broker and start loop    
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(broker_hostname, broker_port, 60)
    client.loop_start()

    hdr = {
        'Content-Type': 'application/json',
        'Authorization': None #not using HTTP secure
    }

    while True:   
        
       
        #the http posting process
        
        #print(str(changed))
        if changed==1:
            payload = {
            'time': str(datetime.now()),
            'event': currentstate,
            'room': "vhe_205"
            }
            response = requests.post("http://0.0.0.0:5000/post-event", headers = hdr,
                                data = json.dumps(payload))
            print(response.json())
            changed=0
        #message=""
        time.sleep(1)
