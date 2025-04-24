#include <WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>


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
  #define NAME "Unknown"
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

String websocket_server = SERVER;
String teamName = FACTION_NAME;
String wsPath = "/score_ws/" + teamName;

const uint16_t websocket_port = 80;

#define TFT_SCL   33
#define TFT_SDA   25
#define TFT_RST   26
#define TFT_DC    27
#define TFT_CS    13
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,TFT_DC,TFT_SDA,TFT_SCL,TFT_RST);


// WebSocket Client
WebSocketsClient webSocket;
String receivedScore = ""; // Initially blank

// Function to Update Display
void updateDisplay(String score) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 20);
    tft.print(teamName);
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
            // Do not update the display here.
            // This leaves the last known score (or a blank) on the screen.
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
    webSocket.begin(websocket_server, websocket_port, wsPath);
    
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000); // Try reconnecting every 5 seconds
}

void loop() {
    webSocket.loop();
    delay(100);
}