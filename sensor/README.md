# Dependencies
Open Arduino IDE and go to File -> Preferences and add under `Additional Boards Manager URLs` the URL for the ESP8266 boards: [https://arduino.esp8266.com/stable/package_esp8266com_index.json](https://arduino.esp8266.com/stable/package_esp8266com_index.json)

Then go to Tools -> Manage Libraries and install the following libraries:
* Adafruit SGP30 Library, Version 1.2.2
* Adafruit SHT31 Library, Version 1.2.0 (not 2.x.x)

# Additional Dependencies - Homie backend
Add the libraries listed in the Homie "Getting started" guide, e.g. 1a.: https://homieiot.github.io/homie-esp8266/docs/3.0.0/quickstart/getting-started/

For Homie release 3.0.0:

> Download Homie relase 3.0.0 https://github.com/homieiot/homie-esp8266/releases/tag/v3.0.0
> Load the .zip with Sketch → Include Library → Add .ZIP Library
> 
> Add the Homie dependencies:
> ArduinoJson >= 5.0.8 https://github.com/bblanchon/ArduinoJson
> Bounce2 https://github.com/thomasfredericks/Bounce2
> ESPAsyncTCP >= c8ed544 https://github.com/me-no-dev/ESPAsyncTCP
> AsyncMqttClient https://github.com/marvinroger/async-mqtt-client
> ESPAsyncWebServer https://github.com/me-no-dev/ESPAsyncWebServer
> 
> Some of them are available through the Arduino IDE, with Sketch → Include Library → Manage Libraries. For the others, install it by downloading the .zip on GitHub.

# Configuration
Copy the `config_example.h` to `config.h` and fill out the values.

# Building
In Arduino IDE select the ESP board under Tools:

* Board: NodeMCU 1.0 (ESP-12E) or WEMOS D1 Mini
* CPU frq 80 MHz
* Flash Size: 1M (256K SPIFFS)
* Flash Mode: DOUT
* Flash frq 80 MHz

Then select Sketch -> Upload to flash the firmware to the MCU.

For OTA, select Sketch -> Export compiled binary and look for the resulting `.bin` file. Open [http://ip-of-sensor/update](http://ip-of-sensor/update) and upload the `.bin` file.
