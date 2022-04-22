/*
 * Create a TCP Server on ESP8266. 
 * TCP Socket Server Send Receive Demo
 * 
 * Written by SIN Consulting 2019
*/

#include <ESP8266WiFi.h>

int port = 8888;                        // Define port number
WiFiServer server(port);                // Creates a server that listens for incoming connections on the specified port

//Server connect to WiFi Network
const char *ssid = "Enrico's iPhone";             // WIFI SSID
const char *password = "abcd1234";  // WIFI Password

//const char *ssid = "50-Hesse";             // WIFI SSID
//const char *password = "14941798";  // WIFI Password
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

void loop() 
{
  // listen for incoming clients
  WiFiClient client = server.available();
  
  if (client) {
    //Client handling code
    if(client.connected())
    {
      Serial.println("Client Connected");
    }
    
    while(client.connected()){      
      while(client.available()>0)
      {
        // read data from the connected client
       char client_message = client.read();
       Serial.write(client_message);    
       }
      
      delay(10); //small delay between polling
      
      //send data to the connected client
      while(Serial.available()>0)
      {
        //when I start the LV session it reads properly but then it doesn't, how do I clear the port? 
        // Otherwise a propoer comm protocol should suffice. Also when I have server message declared as new var
        // it doesn't work the same as when I keep repeating it, I wonder why
        char server_message[100] ; 
        byte buffSize = Serial.readBytesUntil('\n', server_message, 100);
        server_message[buffSize] = 0 ;  
        server_message[buffSize+1] = CRLF[0] ; 
        server_message[buffSize+2] = CRLF[1] ;
        client.write(server_message, sizeof(server_message));
//        Serial.print("Server message: ") ;
//        Serial.print(server_message) ; 
//        Serial.print("Size of server message: ") ;
//        Serial.print(sizeof(server_message)) ; 
//        Serial.print(" Size of buffer: ") ;
//        Serial.println(buffSize) ;   
      }
    }
    
    //close the connection to the client
    client.stop();
    Serial.println("Client disconnected");    

  }
}
//===================================================================================================================================
