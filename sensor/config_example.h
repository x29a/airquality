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

// ADVANCED SETTINGS - THE DEFAULTS ARE PROBABLY OK
// number of seconds before starting a new measurement (should be 1Hz)
#define MEASUREMENT_DELAY 1
// number of seconds before taking and saving a new baseline (supposed to be once per hour)
#define BASELINE_DELAY 3600
// number of seconds before uploading the current values to the internet
#define UPLOAD_DELAY 60
