# Dependencies
Open Arduino IDE and go to File -> Preferences and add under `Additional Boards Manager URLs` the URL for the ESP8266 boards: [https://arduino.esp8266.com/stable/package_esp8266com_index.json](https://arduino.esp8266.com/stable/package_esp8266com_index.json)

Then go to Tools -> Manage Libraries and install the following libraries:
* Adafruit SGP30 Library, Version 1.2.2
* Adafruit SHT31 Library, Version 1.2.0 (not 2.x.x)

# Configuration
Copy the `config_example.h` to `config.h` and fill out the values.

# Building
In Arduino IDE select the ESP board under Tools:

* Board: NodeMCU 1.0 (ESP-12E)
* CPU frq 80 MHz
* Flash Size: 1M (256K SPIFFS)
* Flash Mode: DOUT
* Flash frq 80 MHz

Then select Sketch -> Upload to flash the firmware to the MCU.

For OTA, select Sketch -> Export compiled binary and look for the resulting `.bin` file. Open [http://ip-of-sensor/update](http://ip-of-sensor/update) and upload the `.bin` file.