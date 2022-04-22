#include <ESP32Servo.h>

Servo myservo;

int pos = 0;
#if defined(ARDUINO_ESP32S2_DEV)
int servoPin = 17;
#else
int servoPin = 18;
#endif

void setup() {
  // Allow allocation of all timers
  Serial.begin(115200);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 615, 2415); // 
}

void loop() {

  myservo.write(pos);    // tell servo to go to position in variable 'pos'
  delay(15);
  if(Serial.available()>0){
    char server_message[100] ; 
    byte buffSize = Serial.readBytesUntil('\n', server_message, 100);
    server_message[buffSize] = 0 ; 
    if (server_message[0] == 'M') {
      char *s = strtok(server_message, ","); //allows string to be broken into tokens by " ".
      s = strtok(NULL, ",");
      if (s != NULL) pos = (double) atoi(s); //sets variable to received data and converts ASCII to integer if message is not empty 
      Serial.print("M,");
      Serial.println(pos);
//      Serial.print("Setting position to: ");
//      Serial.println(pos);
    }
    else{
      Serial.println("Command not recognized") ; 
    }
  }
}
