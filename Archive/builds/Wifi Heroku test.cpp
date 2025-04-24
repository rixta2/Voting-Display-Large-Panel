#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// Replace these with your actual credentials and settings
#ifndef WIFI_SSID
  #define WIFI_SSID "Unknown_SSID"
#endif

#ifndef WIFI_PASSWORD
  #define WIFI_PASSWORD "Unknown_Password"
#endif

#ifndef FACTION_NAME
  #define FACTION_NAME "Unknown"
#endif

#ifndef SERVER
  #define SERVER "http://example.com/score/"  // Ensure trailing slash if needed
#endif

#define IR_SENSOR_PIN 25  // Connect to IR sensor output
#define LED_PIN 2         // Onboard LED for feedback

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
String factionName = FACTION_NAME;
String server = SERVER;
String url = server + factionName;

unsigned long lastTriggerTime = 0;
const unsigned long debounceDelay = 1000;  // 1 second debounce

void setup() {
    Serial.begin(115200);
    pinMode(IR_SENSOR_PIN, INPUT_PULLUP);  // Use internal pull-up to avoid floating input
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    WiFi.begin(ssid, password);
    Serial.print("[INFO] Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[INFO] WiFi Connected!");
}

void reconnectWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WARNING] WiFi Disconnected! Reconnecting...");
        WiFi.disconnect();
        WiFi.reconnect();
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 10) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n[INFO] Reconnected to WiFi!");
        } else {
            Serial.println("\n[ERROR] Failed to reconnect to WiFi.");
        }
    }
}

void sendIncrement() {
    if (WiFi.status() != WL_CONNECTED) {
        reconnectWiFi();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[ERROR] WiFi still disconnected! Skipping request.");
            return;
        }
    }

    HTTPClient http;
    int attempts = 0;
    int httpCode = -1;

    while (attempts < 3) {
        http.begin(url);
        http.addHeader("Authorization", "qrIL8SMfv2sKznqyZxRZDjWqvqCiPV9NNTaKnVnm308");

        Serial.printf("[INFO] Sending Score Increment Request... Attempt %d\n", attempts + 1);
        httpCode = http.GET();

        if (httpCode == 200) {
            Serial.println("[INFO] Score Incremented Successfully");
            break;
        } else {
            Serial.printf("[ERROR] Failed to Increment Score, Code: %d\n", httpCode);
            attempts++;
            delay(500);
        }

        http.end();
    }

    if (httpCode != 200) {
        Serial.println("[CRITICAL] Failed to increment score after multiple attempts!");
    }
}

void loop() {
    bool sensorTriggered = digitalRead(IR_SENSOR_PIN) == LOW;

    if (sensorTriggered && (millis() - lastTriggerTime > debounceDelay)) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("[INFO] IR Trigger Detected! Sending score increment...");
        sendIncrement();
        lastTriggerTime = millis();
    } else {
        digitalWrite(LED_PIN, LOW);
    }
}
