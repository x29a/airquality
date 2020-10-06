// BACKEND SELECTION
// Use the homie library to publish sensor data via MQTT
#undef BACKEND_HOMIE
// Upload the measurements via HTTP POSE to server
#define BACKEND_HTTP
// Show the current CO2 measurement on a LED strip (NeoPixel/WS2811/WS2812)
#define OUTPUT_LEDS

// i2c pins - default for D1 Mini
const int sclPin = D1;
const int sdaPin = D2;

// SETTINGS FOR HTTP BACKEND
#ifdef BACKEND_HTTP
  // sensor unit id - sent to backend
  String unitId = "sensor01";
  
  // wifi configuration
  const char* wifi_ssid = "wifiSSID";
  const char* wifi_passwort = "wifiPASSWORD";
  
  // endpoint configuration
  // protocol to use
  String endpointProto = "http://";
  // host of endpoint
  const char* endpointHost = "server.tld";
  // port of endpoint
  const int endpointPort = 80;
  // url of endpoint
  String endpointUrl = "/airquality/api/";
  
  String endpointAddress = endpointProto + endpointHost + ":" + endpointPort + endpointUrl;
#endif //BACKEND_HTTP

// No settings for BACKEND_HOMIE required, see Hoie docs for config instructions
//  https://homieiot.github.io/homie-esp8266/docs/2.0.0/configuration/http-json-api/
//  https://homieiot.github.io/homie-esp8266/docs/3.0.0/configuration/json-configuration-file/

// SETTINGS FOR THE LED OUTPUT
#ifdef OUTPUT_LEDS
  #define LEDS_PIN 14 //= D5 on WeeMos D1 Mini, you can use virtually any pin!
  #define LEDS_NUMPIXELS 24
  #define LEDS_COLOR_GREEN Adafruit_NeoPixel::Color(0, 25, 0) // r = 0, g = 25, b = 0; each of 255 max!
  #define LEDS_COLOR_YELLOW Adafruit_NeoPixel::Color(75, 75, 0) // r = 75, g = 75, b = 0; Make it a bit more noticable
  #define LEDS_COLOR_RED Adafruit_NeoPixel::Color(255, 0, 0) // r = 255, g = 0, b = 0; No time to be subtle, make it shine!
  
  #define LEDS_THRESHOLD_YELLOW 800 // [ppm]
  #define LEDS_THRESHOLD_RED 1000 // [ppm]
  #define LEDS_MINVAL 300 // [ppm]
  #define LEDS_MAXVAL 1200 // [ppm]
#endif //OUTPUT_LEDS

// ADVANCED SETTINGS - THE DEFAULTS ARE PROBABLY OK
// number of seconds before starting a new measurement (should be 1Hz)
#define MEASUREMENT_DELAY 1
// number of seconds before taking and saving a new baseline (supposed to be once per hour)
#define BASELINE_DELAY 3600
// number of seconds before uploading the current values to the internet
#define UPLOAD_DELAY 60
