#include <Arduino.h>
#include <FastLED.h>
#include <WebSocketsClient.h>
#include <Ethernet.h>
#include <ArduinoJson.h>

// LED settings
#define LED_PIN     21      // WS2812B data pin
#define NUM_LEDS    93      // Total LEDs in a single panel
#define BRIGHTNESS  50

// Ethernet settings
byte mac[] = { 0xF8, 0xB3, 0x7, 0x2B, 0x1E, 0x2C }; 
IPAddress ip(192, 168, 0, 177);
IPAddress dns(192, 168, 1, 1);
EthernetClient client;

// WebSocket settings (from the provided file)
#ifndef FACTION_NAME
  #define FACTION_NAME "Unknown"
#endif

#ifndef SERVER
  #define SERVER "Unknown"
#endif

String websocket_server = SERVER;
String factionName = FACTION_NAME;
String wsPath = "/score_ws/" + factionName;
const uint16_t websocket_port = 80;

WebSocketsClient webSocket;
CRGB leds[NUM_LEDS];
int lastScore = -1;

// 7-segment LED segment mapping
const int segmentMap[7][4] = {
  {1, 2, 3, 4},   // Top left
  {5, 6, 7, 8},   // Top
  {9, 10, 11, 12}, // Top Right
  {14, 15, 16, 17}, // Bottom Right
  {18, 19, 20, 21}, // Bottom
  {22, 23, 24, 25}, // Bottom Left
  {26, 27, 28, 29}  // Middle
};

// 7-segment number mapping
const byte numbers[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 0, 1, 1, 0, 0, 0}, // 1
  {0, 1, 1, 0, 1, 1, 1}, // 2
  {0, 1, 1, 1, 1, 0, 1}, // 3
  {1, 0, 1, 1, 0, 0, 1}, // 4
  {1, 1, 0, 1, 1, 0, 1}, // 5
  {1, 1, 0, 1, 1, 1, 1}, // 6
  {0, 1, 1, 1, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 1, 0, 1}  // 9
};

void displayDigit(int digit) {
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  if (digit >= 0 && digit <= 9) {
    for (int segment = 0; segment < 7; segment++) {
      if (numbers[digit][segment]) {
        for (int i = 0; i < 14; i++) {
          if (segmentMap[segment][i] < NUM_LEDS) {
            leds[segmentMap[segment][i]] = CRGB::Red;  
          }
        }
      }
    }
  }

  FastLED.show();
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  if (type == WStype_TEXT) {
    Serial.printf("[WebSocket] Message received: %s\n", payload);

    int newScore = atoi((char*)payload);  // Convert payload to integer
    if (newScore >= 0 && newScore <= 9 && newScore != lastScore) {
      Serial.printf("[INFO] Score updated: %d\n", newScore);
      lastScore = newScore;
      displayDigit(newScore);
    }
  } else if (type == WStype_DISCONNECTED) {
    Serial.println("[WebSocket] Disconnected. Reconnecting...");
    webSocket.begin(websocket_server.c_str(), websocket_port, wsPath.c_str());
  } else if (type == WStype_CONNECTED) {
    Serial.println("[WebSocket] Connected to server.");
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize Ethernet
  Serial.println("[INFO] Initializing Ethernet...");
  Ethernet.begin(mac, ip, dns);
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("[ERROR] Ethernet cable is not connected. Using static IP.");
  }
  delay(1000);  // Allow Ethernet to stabilize
  Serial.print("[DEBUG] Local IP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("[DEBUG] DNS Server: ");
  Serial.println(Ethernet.dnsServerIP());
  Serial.println("[INFO] Ethernet Initialized!");

  // Initialize FastLED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Initialize WebSocket
  Serial.println("[INFO] Connecting to WebSocket server...");
  webSocket.begin(websocket_server.c_str(), websocket_port, wsPath.c_str());
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  Serial.println("[INFO] WebSocket initialized.");
}

void loop() {
  webSocket.loop();  // Handle WebSocket communication
  delay(100);
}
