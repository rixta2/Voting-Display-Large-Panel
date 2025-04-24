#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ** WiFi Credentials **
const char* ssid = "Pixel_4111";
const char* password = "Bigboygus";
const char* serverUrl = "http://questival.bid/get_score?name=Artificers";

// ** TFT Screen Setup **
#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    32
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

int lastScore = -1;  // Track last displayed score

void displayOnTFT(const String& score) {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE, ST7735_BLACK);
  
  // Display title "Artificers:" centered at the top
  tft.setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds("Artificers", 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, 10);
  tft.print("Artificers");
  
  // Display the score centered below the title
  tft.setTextSize(4);  // Larger text size for score
  tft.getTextBounds(score.c_str(), 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((tft.width() - w) / 2, (tft.height() - h) / 2);
  tft.print(score);
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

  // ** Connect to WiFi **
  WiFi.begin(ssid, password);
  Serial.print("[INFO] Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[INFO] WiFi Connected!");

  // ** Initialize TFT Screen **
  tft.initR(INITR_144GREENTAB);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);

  // ** Get and Display Initial Score **
  lastScore = getScoreFromServer();
  if (lastScore != -1) {
    displayOnTFT(String(lastScore));
  } else {
    displayOnTFT("No Data");
  }
}

void loop() {
  int newScore = getScoreFromServer();
  delay(1000);
  
  Serial.print("[DEBUG] Last Score: ");
  Serial.print(lastScore);
  Serial.print(" | New Score: ");
  Serial.println(newScore);

  if (newScore != lastScore) {
    Serial.println("[INFO] Score changed. Updating display.");
    lastScore = newScore;
    displayOnTFT(String(newScore));
  } else {
    Serial.println("[INFO] No change in score.");
  }

  delay(2000);
}
