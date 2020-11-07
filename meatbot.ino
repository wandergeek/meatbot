#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ET73.h>

#include "config.h"
#include "ssl.h"
 

char payload[100];

#ifndef ESP32
#pragma message(THIS CODE IS FOR ESP32 ONLY!)
#error Select ESP32 board.
#endif

WiFiMulti WiFiMulti;
ET73 meatProbe(MEAT_PROBE_PIN, 20, 200, false);  


void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("x"));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}


void postToES(float temp) {
WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(rootCA);
    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
  
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, esEndpoint + "/" + esIndex + "/_doc/")) {  // HTTPS
        https.addHeader("Content-Type", "application/json");
        sprintf(payload, "{ \"temperatureC\" : %.1f }", temp);
        Serial.print("POSTING: ");
        Serial.println(payload);
        int httpCode = https.POST(payload);
  
        if (httpCode > 0) {
          Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
  
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY || httpCode == HTTP_CODE_CREATED ) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }

      // End extra scoping block
    }
  
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
}

void setup() {
  pinMode(indicatorLEDPin, OUTPUT);     
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(wifiAP, wifiPassword);
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");
  setClock();  
}

void loop() {
  if(millis() % (reportIntervalSeconds * 1000) == 0) {
    digitalWrite(indicatorLEDPin, HIGH);
    float tempC = meatProbe.getTemp_C();  
    postToES(tempC);
    digitalWrite(indicatorLEDPin, LOW);
  }
}
