/*
 * Create a TCP Server on ESP8266. 
 * TCP Socket Server Send Receive Demo
 * 
 * Written by SIN Consulting 2019
*/

#include "WiFi.h" // ESP32 WiFi include

int port = 8888;                        // Define port number
WiFiServer server(port);                // Creates a server that listens for incoming connections on the specified port

const char *ssid = "50-Hesse";             // WIFI SSID
const char *password = "14941795";  // WIFI Password
const char CRLF[2] = {0x0D, 0x0A} ; 

int count=0;

//=====================================================================================================================================

void setup() 
{
  Serial.begin(115200);                 // Opens serial port, then sets data rate (bps)
  
  WiFi.mode(WIFI_STA);                  // Set ESP8266 to Station (STA) mode
  WiFi.begin(ssid, password);           // Connect to WiFi
 
  // Wait for connection  
  Serial.println("Connecting to Wifi");
  while (WiFi.status() != WL_CONNECTED) // While not connected, print ... to serial monitor
  {   
    delay(500);
    Serial.print(".....");
    delay(500);
  }

  // Once connected, print to serial monitor WiFi details
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  

  // Start listening for clients
  server.begin();
  Serial.print("Open Telnet and connect to IP:");
  Serial.print(WiFi.localIP());
  Serial.print(" on port ");
  Serial.println(port);
}

//=====================================================================================================================================

void loop() {
  
    if(Serial.available()>0)
    {
      char server_message[100] ; 
      byte buffSize = Serial.readBytesUntil('\n', server_message, 100);
      server_message[buffSize] = 0 ; 
      Serial.println(server_message);
    }
    delay(200);

}

//===================================================================================================================================
