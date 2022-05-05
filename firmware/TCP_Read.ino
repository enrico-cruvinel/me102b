/*
 * Create a TCP Server on ESP8266. 
 * TCP Socket Server Send Receive Demo
 * 
 * Written by SIN Consulting 2019
*/

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

int port = 8888;                        // Define port number
WiFiServer server(port);                // Creates a server that listens for incoming connections on the specified port
WiFiClient client; 

//Server connect to WiFi Network
const char *ssid = "Enrico's iPhone"; // WIFI SSID
const char *password = "abcd1234";  // /WIFI Password

const char CRLF[2] = {0x0D, 0x0A} ; 

byte read_until(char c, char *buff, int len){
  byte i = 0 ; 
  char r;
  while(i <= len && client.available()>0) {
    r = client.read();
    if (r == c) break ; 
    buff[i] = r; 
    i++;
  }
  return i;
}

//=====================================================================================================================================

void setup() 
{
  Serial.begin(115200);                 // Opens serial port, then sets data rate (bps)
 
  WiFi.mode(WIFI_STA);                  // Set ESP8266 to Station (STA) mode
  WiFi.begin(ssid, password);           // Connect to WiFi
 
  // Wait for connection  
  Serial.print("Connecting to ");
  Serial.println(ssid);
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
  client = server.available();
  
  if (client) {
    //Client handling code
    if(client.connected())
    {
      Serial.println("Client Connected");
    }
    
    while(client.connected()){      
      while(client.available()>0){
        char client_message[100];
        byte buffSize = read_until('\n', client_message, 100) ;
        client_message[buffSize] = 0;
        Serial.println(client_message);
//        client_message[buffSize+1] = '\n';
        client.write(client_message, sizeof(client_message));
      }
    }
    
    //close the connection to the client
    client.stop();
    Serial.println("Client disconnected");    

  }
}
//===================================================================================================================================
