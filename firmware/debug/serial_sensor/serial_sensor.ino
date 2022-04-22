float sensor_reading = 0 ; 
unsigned long t = 0 ; 
char message[100] ; 

void setup() 
{
  Serial.begin(115200);            
}

//=====================================================================================================================================

void loop() {

    t = millis() / 1e3 ; 
    sensor_reading = sin(t) ; 
    if(Serial.available()>0)
    {
      char server_message[100] ; 
      byte buffSize = Serial.readBytesUntil('\n', server_message, 100);
      server_message[buffSize] = 0 ;
      char *s = strtok(server_message, " "); //allows string to be broken into tokens by " ".
      if (s[0] == 'V' && s[1] == '1') {
        sprintf(message, "V1%f", sensor_reading) ; 
        Serial.println(message); 
      }else{
        Serial.println("Command not recognized") ; 
      }
    }
    Serial.println(sensor_reading); 

}

//===================================================================================================================================
