#include "WiFi.h"

void setup() {
  Serial.begin(115200);

  // Get the MAC address of the ESP32
  String macAddress = WiFi.macAddress();
  Serial.println("ESP32 MAC Address: " + macAddress);
}

void loop() {
  // Nothing to do here
}
