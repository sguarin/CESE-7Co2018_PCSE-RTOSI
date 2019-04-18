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

4. Connect to Wifi network

  SSID XXXX
  secret miclave00

5. Open url in browser

http://onyx.local or http://192.168.4.1
