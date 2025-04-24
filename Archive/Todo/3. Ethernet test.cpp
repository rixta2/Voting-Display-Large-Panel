/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-ethernet
 */

 #include <SPI.h>
 #include <Ethernet.h>
 
 // replace the MAC address below by the MAC address printed on a sticker on the Ethernet Shield or module
 byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };
 
 EthernetClient client;
 
 int HTTP_PORT = 80;
 String HTTP_METHOD = "GET";  // or POST
 char HOST_NAME[] = "example.com";
 String PATH_NAME = "/";
 
 // Variable to hold IP Address
 IPAddress ip;
 
 void setup() {
   // Initialize Serial Monitor
   Serial.begin(115200);
   delay(1000);
   Serial.println("ESP32 - Ethernet Tutorial");
 
   // Initialize the Ethernet shield using DHCP
   if (Ethernet.begin(mac) == 0) {
     Serial.println("Failed to obtain an IP address");
 
     // Check for Ethernet hardware presence
     if (Ethernet.hardwareStatus() == EthernetNoHardware)
       Serial.println("Ethernet shield was not found");
 
     // Check if Ethernet cable is connected
     if (Ethernet.linkStatus() == LinkOFF)
       Serial.println("Ethernet cable is not connected.");
 
     // Halt the program
     while (true)
       ;
   }
 
   // Get and print the assigned dynamic IP address
   ip = Ethernet.localIP();
   Serial.print("Assigned IP address: ");
   Serial.println(ip);
 
   // Connect to the server on port 80
   if (client.connect(HOST_NAME, HTTP_PORT)) {
     // If connected, send an HTTP request
     Serial.println("Connected to server");
     client.println(HTTP_METHOD + " " + PATH_NAME + " HTTP/1.1");
     client.println("Host: " + String(HOST_NAME));
     client.println("Connection: close");
     client.println();  // End HTTP header
 
     // Read the server response
     while (client.connected()) {
       if (client.available()) {
         char c = client.read();
         Serial.print(c);  // Print server's response to Serial Monitor
       }
     }
 
     // Disconnect from the server
     client.stop();
     Serial.println();
     Serial.println("Disconnected from server");
   } else {
     Serial.println("Connection failed");
   }
 }
 
 void loop() {
   // Nothing to do in the loop for now
 }
 