/*
  DHCP-based IP printer

  This sketch uses the DHCP extensions to the Ethernet library
  to get an IP address via DHCP and print the address obtained.
  using an Arduino WIZnet Ethernet shield.

  Circuit:
   Ethernet shield attached to standard SPI pins (ESP32: GPIO 23, 19, 18, and CS on GPIO 5)

  created 12 April 2011
  modified 9 Apr 2012
  by Tom Igoe
  modified 02 Sept 2015
  by Arturo Guadalupi
  updated 6 Apr 2025
  for ESP32 compatibility
 */

 #include <SPI.h>
 #include <Ethernet.h>
 
 // Enter a MAC address for your controller below.
 // Newer Ethernet shields have a MAC address printed on a sticker on the shield
 byte mac[] = {
   0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
 };
 
 void setup() {
   // Configure the CS pin for the Ethernet shield (ESP32 default: GPIO 5)
   Ethernet.init(5);  // Set CS pin to GPIO 5 for ESP32
 
   // Open serial communications and wait for port to open:
   Serial.begin(115200);
   while (!Serial) {
     ; // wait for serial port to connect. Needed for native USB port only
   }
 
   // Start the Ethernet connection:
   Serial.println("Initialize Ethernet with DHCP:");
   if (Ethernet.begin(mac) == 0) {
     Serial.println("Failed to configure Ethernet using DHCP");
     if (Ethernet.hardwareStatus() == EthernetNoHardware) {
       Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
     } else if (Ethernet.linkStatus() == LinkOFF) {
       Serial.println("Ethernet cable is not connected.");
     }
     // No point in carrying on, so do nothing forevermore:
     while (true) {
       delay(1);
     }
   }
   // Print your local IP address:
   Serial.print("My IP address: ");
   Serial.println(Ethernet.localIP());
 }
 
 void loop() {
   switch (Ethernet.maintain()) {
     case 1:
       // Renewed fail
       Serial.println("Error: renewed fail");
       break;
 
     case 2:
       // Renewed success
       Serial.println("Renewed success");
       // Print your local IP address:
       Serial.print("My IP address: ");
       Serial.println(Ethernet.localIP());
       break;
 
     case 3:
       // Rebind fail
       Serial.println("Error: rebind fail");
       break;
 
     case 4:
       // Rebind success
       Serial.println("Rebind success");
       // Print your local IP address:
       Serial.print("My IP address: ");
       Serial.println(Ethernet.localIP());
       break;
 
     default:
       // Nothing happened
       break;
   }
 }