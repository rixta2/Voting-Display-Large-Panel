#include <Ethernet.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <FastLED.h>
#include <SPI.h>

// Ethernet settings
#define ETH_CS 12  // SCS pin
#define ETH_MISO 19
#define ETH_MOSI 23
#define ETH_SCK  18

byte mac[] = { 0xF8, 0xB3, 0x07, 0x2B, 0x1E, 0x2C };
IPAddress ip(192, 168, 0, 177);
IPAddress dns(192, 168, 1, 1);
EthernetClient ethClient;
WebSocketsClient webSocket;

// Constants
#define NUM_LEDS 30
#define BRIGHTNESS 128
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define PANEL_PIN1 4
#define PANEL_PIN2 25
#define PANEL_PIN3 26
#define PANEL_PIN4 27

// LED arrays for each panel
CRGB panel1[NUM_LEDS];
CRGB panel2[NUM_LEDS];
CRGB panel3[NUM_LEDS];
CRGB panel4[NUM_LEDS];

String receivedScore = "";

// 7-segment LED mapping (LEDs 0 and 13 do not make up segments)
const int segmentMap[7][4] = {
  { 1, 2, 3, 4 },     // Top left
  { 5, 6, 7, 8 },     // Top
  { 9, 10, 11, 12 },  // Top Right
  { 14, 15, 16, 17 }, // Bottom Right
  { 18, 19, 20, 21 }, // Bottom
  { 22, 23, 24, 25 }, // Bottom Left
  { 26, 27, 28, 29 }  // Middle
};

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

void clearDisplay() {
  fill_solid(panel1, NUM_LEDS, CRGB::Black);
  fill_solid(panel2, NUM_LEDS, CRGB::Black);
  fill_solid(panel3, NUM_LEDS, CRGB::Black);
  fill_solid(panel4, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void displayDigit(int digit, CRGB* strip) {
  fill_solid(strip, NUM_LEDS, CRGB::Black);

  if (digit >= 0 && digit <= 9) {
    for (int segment = 0; segment < 7; segment++) {
      if (numbers[digit][segment]) {
        for (int i = 0; i < 4; i++) {
          int ledIndex = segmentMap[segment][i];
          if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
            strip[ledIndex] = CRGB::Green;
          }
        }
      }
    }
  }
  FastLED.show();
}

void updateDisplay(String score) {
  while (score.length() < 4) score = "0" + score;
  displayDigit(score[0] - '0', panel1);
  displayDigit(score[1] - '0', panel2);
  displayDigit(score[2] - '0', panel3);
  displayDigit(score[3] - '0', panel4);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      Serial.println("[INFO] Connected to WebSocket server");
      break;
    case WStype_TEXT: {
      receivedScore = String((char*)payload);
      Serial.print("[INFO] Received Score: ");
      Serial.println(receivedScore);
      int score = receivedScore.toInt();
      if (score >= 0 && score <= 9999) {
        updateDisplay(String(score));
      }
      break;
    }
    case WStype_DISCONNECTED:
      Serial.println("[ERROR] Disconnected from WebSocket server");
      clearDisplay();  // Clear the display when disconnected
      break;
  }
}

void setup() {
  Serial.begin(115200);

  // Setup custom SPI for Ethernet
  SPI.begin(ETH_SCK, ETH_MISO, ETH_MOSI, ETH_CS);
  Ethernet.init(ETH_CS);  // Set CS pin before begin
  Ethernet.begin(mac, ip, dns);
  delay(1000);
  Serial.print("Ethernet IP is: ");
  Serial.println(Ethernet.localIP());

  // Connect WebSocket with Authorization header
  webSocket.begin("questival.bid", 80, "/score_ws/nocturne");  // Server URL
  webSocket.setAuthorization("qrIL8SMfv2sKznqyZxRZDjWqvqCiPV9NNTaKnVnm308");  // Set the Authorization header
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  // Setup LED panels
  FastLED.addLeds<LED_TYPE, PANEL_PIN1, COLOR_ORDER>(panel1, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, PANEL_PIN2, COLOR_ORDER>(panel2, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, PANEL_PIN3, COLOR_ORDER>(panel3, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, PANEL_PIN4, COLOR_ORDER>(panel4, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  clearDisplay();  // Initialize with all LEDs off
}

void loop() {
  webSocket.loop();
}
