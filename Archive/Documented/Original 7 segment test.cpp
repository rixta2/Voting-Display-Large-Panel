#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define LED_PIN     2      // WS2812B data pin
#define NUM_LEDS    56     // 7 segments × 8 LEDs each
#define BRIGHTNESS  50

const char* ssid = "gusmanor";  
const char* password = "Bigboygus";  
const char* serverUrl = "http://questival.bid/get_score?name=Artificers";  

CRGB leds[NUM_LEDS];
int lastScore = -1;  

const int segmentMap[7][4] = {
  { 0, 1, 2, 3},   
  { 8, 9, 10, 11},   
  {16, 17, 18, 19},   
  {24, 25, 26, 27},   
  {32, 33, 34, 35},   
  {40, 41, 42, 43},   
  {48, 49, 50, 51}    
};

const byte numbers[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, 
  {1, 1, 0, 0, 0, 0, 0}, 
  {1, 0, 1, 1, 0, 1, 1}, 
  {1, 1, 1, 0, 0, 1, 1}, 
  {1, 1, 0, 0, 1, 0, 1}, 
  {0, 1, 1, 0, 1, 1, 1}, 
  {0, 1, 1, 1, 1, 1, 1}, 
  {1, 1, 0, 0, 0, 1, 0}, 
  {1, 1, 1, 1, 1, 1, 1}, 
  {1, 1, 1, 0, 1, 1, 1}  
};

void displayDigit(int digit) {
  Serial.print("[INFO] Updating Display to show: ");
  Serial.println(digit);

  FastLED.clear();
  
  if (digit >= 0 && digit <= 9) {
    for (int segment = 0; segment < 7; segment++) {
      if (numbers[digit][segment]) {
        Serial.print("[DEBUG] Lighting up segment: ");
        Serial.println(segment);
        for (int i = 0; i < 8; i++) {
          leds[segmentMap[segment][i]] = CRGB::Red;  
        }
      }
    }
  }

  FastLED.show();
  delay(50);  // Ensure FastLED updates properly
}

int getScoreFromServer() {
  HTTPClient http;
  http.begin(serverUrl);
  
  int httpCode = http.GET();
  if (httpCode == 200) {
    String payload = http.getString();
    Serial.println("[HTTP] Response: " + payload);

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
      Serial.print("[ERROR] JSON Parsing Error: ");
      Serial.println(error.c_str());  
      return lastScore;  
    }

    if (!doc.containsKey("score") || !doc["score"].is<int>()) {
      Serial.println("[ERROR] Invalid JSON response, keeping last score.");
      return lastScore;
    }

    int score = doc["score"];  
    Serial.print("[INFO] Fetched Score: ");
    Serial.println(score);

    return score;
  } else {
    Serial.print("[ERROR] HTTP Request failed, code: ");
    Serial.println(httpCode);
    http.end();
    return lastScore;  
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

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  lastScore = getScoreFromServer();
  displayDigit(lastScore);
}

void loop() {
  int newScore = getScoreFromServer();
  delay(1000);
  
  Serial.print("[DEBUG] Last Score: ");
  Serial.print(lastScore);
  Serial.print(" | New Score: ");
  Serial.println(newScore);

  if (newScore != lastScore && newScore >= 0 && newScore <= 9) {
    Serial.println("[INFO] Score changed. Updating display.");
    lastScore = newScore;
    displayDigit(newScore);
  } else {
    Serial.println("[INFO] No change in score.");
  }

  delay(10000);
}
