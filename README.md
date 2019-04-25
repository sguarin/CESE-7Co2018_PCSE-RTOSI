# CESE-7Co2018_PCSE-RTOSI
# PlatformIo quick installation

  ```
  pip install --user platformio
  platformio platform install espressif32
  ```

# Compile and burn firmware

1. Reset ESP from scratch

  * En Linux:
    
  ```
  ~/.platformio/packages/tool-esptool/esptool -ce -v -cp /dev/ttyUSB0
  ```

  * En Windows
    
  ```
  .platformio/packages/tool-esptool/esptool -ce -v -cp COM3
  ```

2. Upload data FS

  ```
  platformio run -t uploadfs --upload-port /dev/ttyUSB0
  ```

3. Compile and upload FW

  ```
  platformio run -t upload --upload-port /dev/ttyUSB0
  ```

4. Board will connect to Wifi network and mqtt broker defined in main.h

  ```
  #define WIFI_SSID "NONAME 2.4GHz.-"
  #define WIFI_PASSWORD "XXX"
  #define MQTT_HOST "mine.sguarin.com.ar"
  #define MQTT_USER "sadpefe_client"
  #define MQTT_PASSWORD "sadpefe_client00"
  ```
  
