#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ThingSpeak.h>
#include <WiFiClient.h>

//////////////////////
// WiFi Definitions //
//////////////////////

extern const char WiFiSSID[];
extern const char WiFiPSK[];

extern unsigned long myChannelNumber;
extern const char * myWriteAPIKey;

/////////////////
// Post Timing //
/////////////////
const unsigned long postRate = 30000;
unsigned long lastPost = 0;

#define ONE_WIRE_BUS 2  // DS18B20 pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

WiFiClient client;

float GetTemperature(bool* bSuccess) {
  float temp;
  DS18B20.requestTemperatures(); 
  temp = DS18B20.getTempCByIndex(0);
  *bSuccess = (temp > -127.0f && temp < 85.0f);
  return temp;
}

void setup() {
  Serial.begin(115200);
  Serial.println("");

  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());

  ConnectToWifi();
  
  ThingSpeak.begin(client);
}

void loop() {
  if (lastPost + postRate <= millis())
  {
	  lastPost = millis();
	  bool bSuccess = false;
	  float temp = -150.0f;
	  temp = GetTemperature(&bSuccess);
	  if (bSuccess)
	  {
		  Serial.print("Writing to thingspeak: ");
		  Serial.println(temp);
		  ThingSpeak.writeField(myChannelNumber, 1, temp, myWriteAPIKey);
	  }
  }
  delay(1000); 
}

void ConnectToWifi() {
  Serial.printf("Connecting to wifi");
  WiFi.begin(WiFiSSID, WiFiPSK);
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

