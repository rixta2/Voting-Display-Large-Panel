#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// WiFi Credentials
const char* ssid = "gusmanor";
const char* password = "Bigboygus";

// WebSocket Server Details
const char* websocket_server = "questival-node-server-8ca29fd63e0a.herokuapp.com";
const uint16_t websocket_port = 80;

// TFT Display Pins
#define TFT_CS    5
#define TFT_RST   4
#define TFT_DC    32

// Initialize TFT display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// WebSocket Client
WebSocketsClient webSocket;
String receivedScore = "Waiting...";

// Function to Update Display
void updateDisplay(String score) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 20);
    tft.print("Artificers:");
    tft.setCursor(10, 50);
    tft.print(score);
}

// WebSocket Event Handler
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            Serial.println("Connected to WebSocket server");
            break;
        case WStype_TEXT:
            receivedScore = String((char*)payload);
            Serial.print("Received Score: ");
            Serial.println(receivedScore);
            updateDisplay(receivedScore);
            break;
        case WStype_DISCONNECTED:
            Serial.println("Disconnected from WebSocket server!");
            receivedScore = "Disconnected";
            updateDisplay(receivedScore);
            break;
    }
}



void setup() {
    Serial.begin(115200);

    // Initialize TFT display
    tft.initR(INITR_MINI160x80);
    tft.fillScreen(ST77XX_BLACK);
    tft.setRotation(3);

    // Show startup message
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(10, 10);
    tft.print("Connecting...");

    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");

    // Connect to WebSocket Server
    Serial.println("Connecting to WebSocket server...");
    webSocket.begin(websocket_server, websocket_port, "/");
    
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000); // Try reconnecting every 5 seconds
}

void loop() {
    webSocket.loop();
    delay(100);
}
