#include <MQTTClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#define WIFI_SSID "secret"
#define WIFI_PASS "secret"

#define MQTT_HOST "10.10.0.1"
#define MQTT_CLIENT_ID "red-button"
#define MQTT_USER "secret"
#define MQTT_PASS "secret"

#define PIN_LED 2
#define PIN_RED 0
#define PIN_GREEN 3

#define GREEN_HIGH 1
#define RED_HIGH 1

#define PING_DELAY 30000

WiFiClient wifiClient;
MQTTClient mqtt;

void connectWiFi() {
  Serial.print("\nConnecting to WiFi ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  mqtt.begin(MQTT_HOST, wifiClient);
}

void connectMQTT() {
  if (mqtt.connected()) return;

  Serial.print("Waiting for WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connecting to MQTT broker ");
  while (!mqtt.connect((char *)MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("MQTT connected");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setup");
  
  pinMode(PIN_RED, INPUT);
  digitalWrite(PIN_RED, HIGH);
  pinMode(PIN_GREEN, INPUT);
  digitalWrite(PIN_GREEN, HIGH);
  pinMode(PIN_LED, OUTPUT);
}


long lastPing = 0;

void loop() {
  digitalWrite(PIN_LED, LOW);
  connectWiFi();
  connectMQTT();
  while (mqtt.connected()) {
    digitalWrite(PIN_LED, HIGH);
    if (digitalRead(PIN_RED) == 1) {
      Serial.println("RED");
      digitalWrite(PIN_LED, LOW);
      mqtt.publish("actions", "api/switch");
      delay(1000);
      while (digitalRead(PIN_RED) == 1) {
        delay(100);
      }
    }

    if (digitalRead(PIN_GREEN) == 1) {
      Serial.println("GREEN");
      digitalWrite(PIN_LED, LOW);
      mqtt.publish("actions", "api/music/start");
      delay(2000);
      while (digitalRead(PIN_GREEN) == 1) {
        delay(100);
      }
    }

    if ((millis() - lastPing) > PING_DELAY) {
      lastPing = millis();
      mqtt.publish("actions", "ping");
    }
    
    delay(50);
  }
}

void messageReceived(String topic, String payload, char *bytes, unsigned int len) {
  
}