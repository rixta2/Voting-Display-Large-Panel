#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

// ** RFID Reader Setup **
#define RFID_SDA 21  // SDA pin for PN532
#define RFID_SCL 22  // SCL pin for PN532
Adafruit_PN532 nfc(RFID_SDA, RFID_SCL);

// ** Variables **
String UID = "";

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
  // ** Initialize Serial Monitor **
  Serial.begin(115200);
  Serial.println("Initializing PN532...");

  // ** Initialize RFID Reader **
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Failed to find PN532 chip. Check connections.");
    while (1); // Halt program
  }
  nfc.SAMConfig();
  Serial.println("PN532 initialized. Waiting for NFC...");
}

void loop() {
  // Read RFID
  UID = readRFID();

  if (UID != "") {  // If a card is detected
    Serial.print("Scanned UID: ");
    Serial.println(UID);

    delay(3000);  // Wait for 3 seconds before scanning again
  }

  delay(500); // Small delay to prevent rapid polling
}
