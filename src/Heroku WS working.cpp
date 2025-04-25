#include <WiFi.h>
#include <WebSocketsClient.h>
#include <FastLED.h>
#include <string>
#include <iostream>

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
  #define SERVER "Unknown"
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

String websocket_server = SERVER;
String factionName = FACTION_NAME;
String wsPath = "/score_ws/" + factionName;
const uint16_t websocket_port = 80;
unsigned long lastLoop = 0;
int currentDigits[4] = {-1, -1, -1, -1};  // -1 means nothing shown

WebSocketsClient webSocket;
String receivedScore = "";

#define BRIGHTNESS  100
#define NUM_LEDS    97

#define PANEL1_PIN 33  // Thousands
#define PANEL2_PIN 32  // Hundreds
#define PANEL3_PIN 22  // Tens
#define PANEL4_PIN 21   // Units

CRGB leds1[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB leds3[NUM_LEDS];
CRGB leds4[NUM_LEDS];

const int segmentLengths[7] = {14, 13, 14, 14, 14, 15, 13};
int previousScore = 0;


// 7-segment LED mapping (excluding filler LEDs 0 and 13)
const int segmentMap[7][15] = {
  { 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54 },   // Top left 1
  { 36, 37, 38, 39, 40, 55, 56, 57, 58, 59, 60, 61, 62 },   // Top 2
  { 30, 31, 32, 33, 34, 35, 63, 64, 65, 66, 67, 68, 69, 70 },   // Top Right 3
  { 24, 25, 26, 27, 28, 29, 71, 72, 73, 74, 75, 76, 77, 78 },   // Bottom Right 4
  { 19, 20, 21, 22, 23, 79, 80, 81, 82, 83, 84, 85, 86, 87 },   // Bottom 5
  { 13, 14, 15, 16, 17, 18, 88, 89, 90, 91, 92, 93, 94, 95, 96 },      // Bottom Left 6 
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }            // Middle 7
};

// Digit-to-segment mapping (1 = ON, 0 = OFF)
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

void displayDigit(int digit, CRGB leds[]) {
    if (digit >= 0 && digit <= 9) {
      for (int segment = 0; segment < 7; segment++) {
        for (int i = 0; i < segmentLengths[segment]; i++) {
          leds[segmentMap[segment][i]] = numbers[digit][segment] ? LED_COLOR : CRGB::Black;
      }
      }
    } else {
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
    }

  }

  void updateDisplay(int score_int) {
    Serial.print("Inside update display");
    std::string score = std::to_string(score_int);

    while (score.length() < 4) {
      score = ' ' + score; // Add a blank space to the right
    }

    for (size_t i = 0; i < score.length() ; i++) {
        if(score[i] == ' ') {
          switch(i) {
            case 0:
              for (int a = 0; a < NUM_LEDS; a++) {
                leds1[a] = CRGB::Black;
              }
                break;
            case 1:
            for (int a = 0; a < NUM_LEDS; a++) {
              leds2[a] = CRGB::Black;
            }
              break;
            case 2:
            for (int a = 0; a < NUM_LEDS; a++) {
              leds3[a] = CRGB::Black;
            }
              break;
            case 3:
            for (int a = 0; a < NUM_LEDS; a++) {
              leds4[a] = CRGB::Black;
            }
              break;
        }
          continue;
        }
        int displayDigitValue = score[i] - '0';
        Serial.print("Display digit value: ");
        Serial.println(displayDigitValue);
        Serial.print("score[i]: ");
        Serial.println(score[i]);

        currentDigits[i] = displayDigitValue;
        
        switch(i) {
            case 0:
                displayDigit(displayDigitValue, leds1);
                break;
            case 1:
                displayDigit(displayDigitValue, leds2);
                break;
            case 2:
                displayDigit(displayDigitValue, leds3);
                break;
            case 3:
                displayDigit(displayDigitValue, leds4);
                break;
        }
        
    }

    FastLED.show();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            Serial.println("[INFO] Connected to WebSocket server");
            break;

        case WStype_TEXT: {
            Serial.println("[DEBUG] Received raw WebSocket payload:");
            for (size_t i = 0; i < length; i++) {
                Serial.print((char)payload[i]);
            }
            Serial.println();

            receivedScore = String((char*)payload);
            Serial.print("[INFO] Received Score: ");
            Serial.println(receivedScore);

            int score = receivedScore.toInt();
            if (score >= 0 && score <= 9999 && score != previousScore) {  

                updateDisplay(score);  
                previousScore = score;
                Serial.print("[INFO] Updated display to score: ");
                Serial.println(score);
            } else {
                Serial.println("[WARNING] Invalid score received, ignoring...");
            }
            break;
        }
        case WStype_PONG:
        case WStype_PING:
            break;

        case WStype_DISCONNECTED:
            Serial.println("[ERROR] Disconnected from WebSocket server!");
            break;

        default:
            Serial.print("[DEBUG] Unhandled WebSocket event type: ");
            Serial.println(type);
            break;
    }
}

void connectWiFi() {
    Serial.print("[INFO] Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[INFO] WiFi Connected!");
}

void connectWebSocket() {
    Serial.println("[INFO] Connecting to WebSocket server...");
    webSocket.begin(websocket_server, websocket_port, wsPath);
    webSocket.enableHeartbeat(15000, 3000, 2); //is this necessary?
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000); //how does this work?
}

void setup() {
    Serial.begin(115200);

    FastLED.addLeds<WS2812B, PANEL1_PIN, GRB>(leds1, NUM_LEDS);
    FastLED.addLeds<WS2812B, PANEL2_PIN, GRB>(leds2, NUM_LEDS);
    FastLED.addLeds<WS2812B, PANEL3_PIN, GRB>(leds3, NUM_LEDS);
    FastLED.addLeds<WS2812B, PANEL4_PIN, GRB>(leds4, NUM_LEDS);
    
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();

    connectWiFi();
    WiFi.setSleep(false);
    connectWebSocket();
}

void loop() {

    webSocket.loop();
    
    if (millis() - lastLoop > 10000) {
        lastLoop = millis();
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[ERROR] Lost WiFi connection! Attempting reconnect...");
            connectWiFi();
        }
    
        if (!webSocket.isConnected()) {
            Serial.println("[ERROR] Lost WebSocket connection! Attempting reconnect...");
            connectWebSocket();
        }
    }
}
