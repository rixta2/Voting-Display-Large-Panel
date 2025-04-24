#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_ST7735.h>

// ** RFID Reader Setup **



// ** TFT Setup **
//#define TFT_CS     10
//#define TFT_RST    9
//#define TFT_DC     8
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// ** Variables **
String UID = "";

/*
// Function: Display text centered on the TFT screen
void displayCenteredText(const String& message, uint8_t textSize) {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(textSize);
  int16_t x1, y1;
  uint16_t w, h;

  // Calculate text bounds to center the message
  tft.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
  int16_t x = (tft.width() - w) / 2;
  int16_t y = (tft.height() - h) / 2;

  tft.setCursor(x, y);
  tft.print(message);
}
*/

// Function: Read the UID from the NFC chip
String readRFID() {
  uint8_t success;
  uint8_t uid[7] = {0};  // Buffer to store the returned UID
  uint8_t uidLength;
  String scannedUID = "";

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] < 0x10) {
        scannedUID += "0";  // Add leading zero for single-digit hex values
      }
      scannedUID += String(uid[i], HEX);  // Append byte in HEX format
    }
    scannedUID.toUpperCase();
    return scannedUID;
  }
  return ""; // Return empty string if no card detected
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize RFID
  Serial.println("Initializing PN532...");
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Failed to find PN532 chip. Check connections.");
    while (1); // Halt
  }
  nfc.SAMConfig();
  Serial.println("PN532 initialized. Waiting for NFC...");

  // Initialize TFT
  /*
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  displayCenteredText("Waiting for NFC...", 1);
  */
}

void loop() {
  // Read RFID
  UID = readRFID();
  
  if (UID != "") {
    Serial.print("Scanned UID: ");
    Serial.println(UID);

    // Display the UID in large text on the TFT screen
    //displayCenteredText("UID: " + UID, 2);

    delay(1000); // Wait for a few seconds before scanning again
    //displayCenteredText("Waiting for NFC...", 1);
  }

  delay(500); // Small delay to prevent rapid polling
}
