#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

// ** WiFi Credentials from Build Flags **
#ifndef WIFI_SSID
  #define WIFI_SSID "Unknown_SSID"
#endif

#ifndef WIFI_PASSWORD
  #define WIFI_PASSWORD "Unknown_Password"
#endif

#ifndef NAME
  #define NAME "Unknown"
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* teamName = NAME;
const char* websocketServerUrl = "wss://questival-flask-5dc9c4210ee8.herokuapp.com/ws";

// ** SSL Root Certificate (ISRG Root X1) **
// ** SSL Root Certificate **
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFyjCCBLKgAwIBAgIQAkAKOzfB6GNhSzded0ZNrDANBgkqhkiG9w0BAQsFADA8\n" \
"MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRwwGgYDVQQDExNBbWF6b24g\n" \
"UlNBIDIwNDggTTAzMB4XDTI1MDEzMTAwMDAwMFoXDTI2MDMwMTIzNTk1OVowGjEY\n" \
"MBYGA1UEAwwPKi5oZXJva3VhcHAuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A\n" \
"MIIBCgKCAQEAlVucOu0Y4aNjaZNp6ki8QvDYTJvsKmHrTMMWUp0QYJjUw21bwI7H\n" \
"KhdNNmLhgFZf7Aq5QLHqIYlceOEAaqM8JsJwg5AYDD3GmhsTjP1YgkpAKOrbs8KP\n" \
"/DyCsIXra2buOosdp1CSUY1ydaavIxM634jBBkex7Vk2tJdelBCmE6AK+EwEuJiE\n" \
"Ld/U/b/dxsTEG5lM6Flp6fHXSjKyXM+tir7O6jqdMQCmeEf+XcLMeN92ER2WiWwR\n" \
"n29tEPP4XrZa/mH9CAQ6bxObn9Ala+9At4yo1488b+A13+k15VlX4eY3L58YLB8s\n" \
"MNRhtlObIIteELK0FNI0/ZGCWsg8hvPlvwIDAQABo4IC6DCCAuQwHwYDVR0jBBgw\n" \
"FoAUVdkYX9IczAHhWLS+q9lVQgHXLgIwHQYDVR0OBBYEFOH3hQG6tSCjI6Q8+9Ps\n" \
"-----END CERTIFICATE-----\n";

// ** TFT Display Setup **
#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    32
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

WiFiClientSecure secureClient;
WebsocketsClient wsClient;
int lastScore = -1;

void displayOnTFT(const String& score) {
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
    tft.setRotation(3);
  
    tft.setTextSize(1);
    tft.setCursor(10, 10);
    tft.print(teamName);

    tft.setTextSize(2);
    tft.setCursor(20, 80);
    tft.print(score);
}

void onMessageCallback(WebsocketsMessage message) {
    Serial.println("[INFO] WebSocket Message Received: " + message.data());
  
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, message.data());

    if (error) {
        Serial.println("[ERROR] Failed to parse JSON message.");
        return;
    }

    if (doc["name"].is<const char*>() && doc["score"].is<int>()) {
        String receivedName = doc["name"].as<String>();
        int receivedScore = doc["score"].as<int>();

        if (receivedName == teamName) {
            Serial.printf("[INFO] Score updated: %d\n", receivedScore);
            lastScore = receivedScore;
            displayOnTFT(String(lastScore));
        }
    }
}

void setup() {
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    Serial.print("[INFO] Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[INFO] WiFi Connected!");

    tft.initR(INITR_BLACKTAB);
    tft.setRotation(3);
    tft.fillScreen(ST7735_BLACK);
    displayOnTFT("Waiting...");

    // ** Secure Connection Setup **
    secureClient.setCACert(rootCACertificate);
    // secureClient.setInsecure();  // ❗ Uncomment for debugging (bypasses SSL verification)

    Serial.println("[INFO] Waiting before WebSocket connection...");
    delay(2000);  // 🔹 Prevents connection issues due to WiFi instability

    Serial.println("[INFO] Connecting to WebSocket Server...");
    wsClient.onMessage(onMessageCallback);
    
    if (wsClient.connect(websocketServerUrl)) { // ✅ Correct way to connect
        Serial.println("[INFO] WebSocket Connected!");
        JsonDocument doc;
        doc["name"] = teamName;
        String jsonString;
        serializeJson(doc, jsonString);
        wsClient.send(jsonString);
    } else {
        Serial.println("[ERROR] WebSocket Connection Failed!");
    }
}

void loop() {
    wsClient.poll();
    delay(100);
}
