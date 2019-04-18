#!/bin/bash
PORT=/dev/ttyUSB0
SPEED=115200

platformio run -t upload  --upload-port $PORT && platformio  serialports monitor -b $SPEED -p $PORT
