#include <FS.h>
#include <Wire.h>

#include "Adafruit_SGP30.h"
Adafruit_SGP30 sgp;

#include "Adafruit_SHT31.h"
Adafruit_SHT31 sht31;
bool sht31_available = false;

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#include <ESP8266HTTPUpdateServer.h>
ESP8266HTTPUpdateServer httpUpdater;
#include <ESP8266HTTPClient.h>

#include "config.h"


// timestamp of last measurement in seconds
unsigned long int last_measurement = 0;

// timestamp of last baseline
unsigned long int last_baseline = 0;

// timestamp of last upload
unsigned long int last_upload = 0;

// current values
unsigned long current_tvoc = 0;
unsigned long current_eco2 = 0;
unsigned long current_rawh2 = 0;
unsigned long current_raweth = 0;
float current_temp = 0;
float current_relhum = 0;
unsigned long current_abshum = 0;


/* return absolute humidity [mg/m^3] with approximation formula
  @param temperature [°C]
  @param humidity [%RH]
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity)
{
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
  return absoluteHumidityScaled;
}

/* check and wait for Wifi state WL_CONNECTED
  @param timeOutSec [s]
*/
bool isConnected(long timeOutSec)
{
  // delay between checking Wifi state via busywait
  const int delayMs = 200;
  timeOutSec = timeOutSec * 1000;
  int z = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(delayMs);
    if (z == timeOutSec / delayMs)
    {
      return false;
    }
    z++;
  }
  return true;
}

// webserver index handler - shows current values
void handle_index()
{
  String response;

  response += "TVOC=\"" + String(current_tvoc) + "\"\n";
  response += "eCO2=\"" + String(current_eco2) + "\"\n";
  response += "rawH2=\"" + String(current_rawh2) + "\"\n";
  response += "rawEthanol=\"" + String(current_raweth) + "\"\n";
  response += "temp=\"" + String(current_temp) + "\"\n";
  response += "relHum=\"" + String(current_relhum) + "\"\n";
  response += "absHum=\"" + String(current_abshum) + "\"\n";

  Serial.println("sending response for index");
  server.send(200, "text/plain", response);

  return;
}

// webserver status handler - shows mc status (uptime, heap)
void handle_status()
{
  // current timestamp in seconds
  const unsigned long int now = millis() / 1000.0;

  String response;

  response += "id=\"" + unitId + "\"\n";
  response += "uptime=\"" + String(now) + "\"\n";
  response += "free heap=\"" + String(ESP.getFreeHeap()) + "\"\n";

  server.send(200, "text/plain", response);

  return;
}

// webserver restart handler - performs mc restart
void handle_restart()
{
  ESP.restart();
}

// webserver reset handler - wipes flash and performs mc restart
void handle_reset()
{
  SPIFFS.format();
  delay(30000);
  ESP.restart();
}

// send data to server endpoint
int send_data_to_server()
{
  // check if connection is possible at all
  WiFiClient client;
  if (!client.connect(endpointHost, endpointPort))
  {
    Serial.println("connection failed");
    return 0;
  }

  Serial.print("Requesting: ");
  Serial.println(endpointAddress);

  String postData = "id=";
  postData += unitId;
  postData += "&uptime=" + String(millis() / 1000.0);
  postData += "&TVOC=" + String(current_tvoc);
  postData += "&eCO2=" + String(current_eco2);
  postData += "&rawH2=" + String(current_rawh2);
  postData += "&rawEth=" + String(current_raweth);
  postData += "&temp=" + String(current_temp);
  postData += "&relHum=" + String(current_relhum);
  postData += "&absHum=" + String(current_abshum);

  // perform http post
  HTTPClient http;
  http.begin(endpointAddress);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  auto httpCode = http.POST(postData);
  Serial.println(httpCode);
  String payload = http.getString();
  Serial.println(payload);
  http.end();
  Serial.println("closing connection");

  if (httpCode != 200)
  {
    return 0;
  }

  return 1;
}

void setup()
{
  // setup i2c with specific pins
  Wire.begin(sdaPin, sclPin);

  Serial.begin(115200);
  Serial.println("SGP30 test");

  if (!sgp.begin())
  {
    Serial.println("SGP30 not found :(");
    delay(10000);
    ESP.restart();
  }

  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);

  sht31_available = sht31.begin(0x44);
  Serial.print("SHT31 available: ");
  Serial.println(sht31_available);

  // start the file system
  SPIFFS.begin();

  // reading baseline measurement from before
  File f = SPIFFS.open("/baseline.dat", "r");
  if (!f)
  {
    Serial.println("file read open failed");
    Serial.println("Please wait 30 secs for SPIFFS to be formatted");
    SPIFFS.format();
    delay(30000);
  }
  else
  {
    uint16_t eCO2_base = f.readStringUntil('\n').toInt();
    uint16_t TVOC_base = f.readStringUntil('\n').toInt();

    Serial.print("****Load saved Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
    Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);

    // e.g. 0x8B89 & TVOC: 0x856A
    sgp.setIAQBaseline(eCO2_base, TVOC_base);
  }

  // perform some warm-up measurements since an upload will be performed with the next read values (so they should be correct)
  for (int measurement = 0; measurement < 10; ++measurement)
  {
    sgp.IAQmeasure();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_passwort);

  // wait for initial connection which might take some time
  if (!isConnected(15))
  {
    // no connection possible
    ESP.restart();
  }

  // assign webserver handlers
  server.on("/", handle_index);
  server.on("/status", handle_status);
  server.on("/restart", handle_restart);
  server.on("/reset", handle_reset);
  httpUpdater.setup(&server);

  server.begin();

  // tell the server we booted by sending a 0-value request (since no reading has been performed) - like a ping
  if (send_data_to_server())
  {
    last_upload = millis() / 1000.0;
  }
}

void loop()
{
  server.handleClient();

  if (!isConnected(10))
  {
    Serial.println("lost wifi, restarting");
    delay(200);
    ESP.restart();
  }

  // current timestamp in seconds
  const unsigned long now = millis() / 1000.0;

  // difference between valid measurements in seconds
  const unsigned long time_diff = now - last_measurement;

  // check if we should measure again
  if (last_measurement == 0 || time_diff >= MEASUREMENT_DELAY)
  {
    if (sht31_available)
    {
      current_temp = sht31.readTemperature();

      if (current_temp != NAN)
      {
        Serial.print("temp "); Serial.println(current_temp);

        current_relhum = sht31.readHumidity();

        if (current_relhum != NAN)
        {
          current_abshum = getAbsoluteHumidity(current_temp, current_relhum);
          // set the absolute humidity to enable the humditiy compensation for the air quality signals
          sgp.setHumidity(current_abshum);

          Serial.print("relHum: "); Serial.print(current_relhum); Serial.print("%\t");
          Serial.print("absHum: "); Serial.print(current_abshum); Serial.println(" [mg/m^3]");
        }
      }
    }

    if (!sgp.IAQmeasure())
    {
      Serial.println("Measurement failed");
      return;
    }
    Serial.print("TVOC: "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
    Serial.print("eCO2: "); Serial.print(sgp.eCO2); Serial.println(" ppm");

    if (!sgp.IAQmeasureRaw())
    {
      Serial.println("Raw Measurement failed");
      return;
    }
    Serial.print("Raw H2: "); Serial.print(sgp.rawH2); Serial.print(" \t");
    Serial.print("Raw Ethanol: "); Serial.print(sgp.rawEthanol); Serial.println("");

    // check if sensor values are marked as invalid
    if (sgp.TVOC >= 60000)
    {
      Serial.println("invalid sensor values detected, performing softreset of sensor");
      // soft reset failed or no effect (since value still invalid), reboot
      if (!sgp.softReset() || current_tvoc >= 60000)
      {
        // softreset command failed, lets try reboot of MCU
        Serial.println("softreset command failed or no effect, restarting MCU");
        ESP.restart();
      }
      delay(5000);
    }

    // got valid readings, mark this as last measurement
    last_measurement = now;
    current_tvoc = sgp.TVOC;
    current_eco2 = sgp.eCO2;
    current_rawh2 = sgp.rawH2;
    current_raweth = sgp.rawEthanol;

    // difference between uploads in seconds
    const unsigned long time_diff_upload = now - last_upload;

    // check if we should upload again
    if (time_diff_upload >= UPLOAD_DELAY)
    {
      // only mark succesfull data deliveries - otherwise try again asap
      if (send_data_to_server())
      {
        last_upload = now;
      }
    }

    // difference between baselines in seconds
    const unsigned long time_diff_baseline = now - last_baseline;

    // check if we should save a baseline again
    if (time_diff_baseline >= BASELINE_DELAY)
    {
      uint16_t TVOC_base, eCO2_base;
      if (!sgp.getIAQBaseline(&eCO2_base, &TVOC_base))
      {
        Serial.println("Failed to get baseline readings");
        return;
      }
      Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
      Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);

      File f = SPIFFS.open("/baseline.dat", "w");
      if (!f)
      {
        Serial.println("file write open failed");
        return;
      }
      else
      {
        f.print(eCO2_base);
        f.print("\n");
        f.print(TVOC_base);
        f.print("\n");
        f.close();

        last_baseline = now;
      }
    }
  }
}
