#include <Arduino.h>

int a, b, c, d, e, f; 
void decode_message() ; 

void setup() 
{

  Serial.begin(115200);

}

//=====================================================================================================================================
void loop() {
  decode_message();

}


/************************************/
/*          helper functions        */
/************************************/

void decode_message(){
    if(Serial.available()>0){
    char server_message[100] ; 
    byte buffSize = Serial.readBytesUntil('\n', server_message, 100);
    server_message[buffSize] = 0 ; 
    
    // Example: R,S,1,100
    if (server_message[0] == 'W') {
      Serial.println(server_message);
      char *s = strtok(server_message, ","); //allows string to be broken into tokens by ",".
      s = strtok(NULL, ",");
      switch(s[0]){
        case 'S':
          s = strtok(NULL, ",");
          if (s != NULL) a = atoi(s); 
          s = strtok(NULL, ",");
          if (s != NULL) b = atoi(s);
          Serial.println(b);
          break ; 
        case 'A':
          s = strtok(NULL, ",");
          if (s != NULL) a = atoi(s); 
          s = strtok(NULL, ",");
          if (s != NULL) b = atoi(s);
          break ; 
        case 'B':
          s = strtok(NULL, ",");
          if (s != NULL) a = atoi(s); 
          s = strtok(NULL, ",");
          if (s != NULL) b = atoi(s);
          break ; 
        default:
          break ;   
      }
    }
    else if (server_message[0] == 'R'){
      char *s = strtok(server_message, ","); //allows string to be broken into tokens by ",".
      s = strtok(NULL, ",");
      char buff[50] ; 
      switch(s[0]){
        case 'S':
          sprintf(buff, "R,S,%d,%d", a, b) ; 
          Serial.println(buff) ; 
          break ; 
        case 'A':
          sprintf(buff, "R,A,%d,%d", a, b) ; 
          Serial.println(buff) ; 
          break ; 
        case 'B':
          sprintf(buff, "R,B,%d,%d", a, b) ; 
          Serial.println(buff) ; 
          break ; 
        case 'I':
          sprintf(buff, "R,I,%d,%d", a, b) ; 
          Serial.println(buff) ; 
          break ; 
        case 'U':
          sprintf(buff, "R,U,%d,%d", a, b) ; 
          Serial.println(buff) ; 
          break ; 
        default:
          break ;   
      }
      
    }
    else{
      Serial.println("Command not recognized") ; 
    }
  }
}
