#include <Arduino.h>

#define SPR_MOT 26  

const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;
int dutyCycle = 0 ; 

int count=0;
unsigned long on_time=0;
unsigned long start_time, now = 0 ;
//=====================================================================================================================================
void setup() 
{
//  pinMode(SPR_MOT, OUTPUT);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(SPR_MOT, ledChannel); 
  Serial.begin(115200);                 // Opens serial port, then sets data rate (bps)
}

//=====================================================================================================================================
void loop() {

    /************ read message *************/
    if(Serial.available()>0){
      char server_message[100] ; 
      byte buffSize = Serial.readBytesUntil('\n', server_message, 100);
      server_message[buffSize] = 0 ; 
     
      if (server_message[0] == 'M' && server_message[1] == 'O' && server_message[2] == 'T') {
        Serial.println(server_message);
        char *s = strtok(server_message, ","); //allows string to be broken into tokens by " ".
        s = strtok(NULL, ",");
        if (s != NULL && s[0] == 'T') {
          s = strtok(NULL, ",");
          if (s != NULL) on_time = (double) atoi(s); //sets variable to received data and converts ASCII to integer if message is not empty
          s = strtok(NULL, ",");
          s = strtok(NULL, ",");
          if (s != NULL) dutyCycle = (double) atoi(s); //sets variable to received data and converts ASCII to integer if message is not empty
          start_time = millis() ; 
        }
      }
      else{
        Serial.println("Command not recognized") ; 
      }
//    delay(200);
    }
    /************************************/
    if ((millis() - start_time) < on_time){
//      digitalWrite(SPR_MOT, HIGH) ; 
        ledcWrite(ledChannel, dutyCycle);
    } else{
//      digitalWrite(SPR_MOT, LOW) ;
      ledcWrite(ledChannel, 0);
      on_time = 0 ;  
    }
    
      
}
    


//===================================================================================================================================
