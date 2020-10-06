// BACKEND SELECTION
// Use the homie library to publish sensor data via MQTT
#undef BACKEND_HOMIE
// Upload the measurements via HTTP POSE to server
#define BACKEND_HTTP

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

// ADVANCED SETTINGS - THE DEFAULTS ARE PROBABLY OK
// number of seconds before starting a new measurement (should be 1Hz)
#define MEASUREMENT_DELAY 1
// number of seconds before taking and saving a new baseline (supposed to be once per hour)
#define BASELINE_DELAY 3600
// number of seconds before uploading the current values to the internet
#define UPLOAD_DELAY 60
