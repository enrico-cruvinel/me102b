#include <Arduino.h>
#include <ESP32Servo.h>
#include <ESP32Encoder.h>

#define SPR_MOT_PIN 15
#define MOT_A_1 5
#define MOT_A_2 18
#define ENC_A_1 13
#define ENC_A_2 12
#define MOT_B_1 19
#define MOT_B_2 21
#define ENC_B_1 14
#define ENC_B_2 27
#define ECHO_PIN 26
#define TRIG_PIN 25


/******* functions *******/ 
void decode_message() ; 
void update_motors() ; 
void update_encoder() ;
void read_ultrasonic();
void connect_to_WiFi();
byte read_until(char c, char *buff, int len);
/**** data structures ****/
/************************************/
/*          type definitions        */
/************************************/
typedef struct motor_conf_t{
  int enable;
  int dutyCycle; 
} motor_conf_t ; 

typedef struct encoder_data_t{
  float p;
  float v;
  float p_last;
} encoder_data_t ; 

/************************************/
/*          var definitions         */
/************************************/

/******** motors ********/
const int freq = 5000;
const int resolution = 8;

const int mot_a_channel_1 = 5;
const int mot_a_channel_2 = 6;
const int mot_b_channel_1 = 7;
const int mot_b_channel_2 = 8;

motor_conf_t spr_mot_conf = {.enable = 0, .dutyCycle = 0} ;
motor_conf_t mot_a_conf = {.enable = 0, .dutyCycle = 0} ; 
motor_conf_t mot_b_conf = {.enable = 0, .dutyCycle = 0} ;

Servo spr_mot;

int count=0;
unsigned long on_time=0;
unsigned long start_time, now = 0 ;
/***********************/

/******** encoders ********/
ESP32Encoder encoder_A;
ESP32Encoder encoder_B;

encoder_data_t enc_A_data = {.p = 0, .v = 0, .p_last = 0 };
encoder_data_t enc_B_data = {.p = 0, .v = 0, .p_last = 0 };

unsigned long prev_time = 0;
const long dt = 20;
/***********************/

/****** ultrasonic *******/
#define SOUND_SPEED 0.034
long dT;
float distanceCm;
/************************/

/********* wifi *********/
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

int port = 8888;                        // Define port number
WiFiServer server(port);                // Creates a server that listens for incoming connections on the specified port
WiFiClient client ; 
//Server connect to WiFi Network
const char *ssid = "Enrico's iPhone"; // WIFI SSID
const char *password = "abcd1234";  // /WIFI Password

const char CRLF[2] = {0x0D, 0x0A} ; 
/************************/

//=====================================================================================================================================
void setup() 
{

  /*** motors ***/
  
  //servo motor
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  spr_mot.setPeriodHertz(50);    // standard 50 hz servo
  spr_mot.attach(SPR_MOT_PIN, 615, 2415); // 

  //pwm channel
  ledcSetup(mot_a_channel_1, freq, resolution);
  ledcSetup(mot_a_channel_2, freq, resolution);
  ledcSetup(mot_b_channel_1, freq, resolution);
  ledcSetup(mot_b_channel_2, freq, resolution);
  
  //pins
  ledcAttachPin(MOT_A_1, mot_a_channel_1);
  ledcAttachPin(MOT_A_2, mot_a_channel_2);
  ledcAttachPin(MOT_B_1, mot_b_channel_1);
  ledcAttachPin(MOT_B_2, mot_b_channel_2);

  /***********/
  

  /*** encoders ***/
  ESP32Encoder::useInternalWeakPullResistors = UP; // Enable the weak pull up resistors
  encoder_A.attachHalfQuad(ENC_A_1, ENC_A_2); // Attache pins for use as encoder pins
  encoder_A.setCount(0);  // set starting count value after attaching
  encoder_B.attachHalfQuad(ENC_B_1, ENC_B_1); // Attache pins for use as encoder pins
  encoder_B.setCount(0);  // set starting count value after attaching
  /****************/

  /*** ultrasonic ***/
  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN, INPUT); // Sets the echoPin as an Input
  /*****************/
  
  Serial.begin(115200);
  connect_to_WiFi();
  

}

//=====================================================================================================================================
void loop() {

  client = server.available();

    if (client) {
    //Client handling code
    if(client.connected())
    {
      Serial.println("Client Connected");
    }
    
    while(client.connected()){      
      /************ read message *************/
      decode_message() ; 
      /************************************/
    
      /*********** motor write ************/
      update_motors(); 
      /************************************/
        
      /************* encoder **************/
      update_encoder() ; 
      /***********************************/
    
      /************ sensors **************/
      read_ultrasonic();
      /***********************************/
    }
 
    client.stop();
    Serial.println("Client disconnected");    

  }

}

/************************************/
/*          helper functions        */
/************************************/

void decode_message(){
    if(client.available()>0){
      char server_message[100] ; 
      byte buffSize = read_until('\n', server_message, 100);
      server_message[buffSize] = '\r' ;
      server_message[buffSize+1] = '\n' ;
      server_message[buffSize+2] = 0 ; 
      // Example: R,S,1,100
      if (server_message[0] == 'W') {
        Serial.print(server_message);
        client.write(server_message, strlen(server_message));
        char *s = strtok(server_message, ","); //allows string to be broken into tokens by ",".
        s = strtok(NULL, ",");
        switch(s[0]){
          case 'S':
            s = strtok(NULL, ",");
            if (s != NULL) spr_mot_conf.enable = atoi(s); 
            s = strtok(NULL, ",");
            if (s != NULL) spr_mot_conf.dutyCycle = atoi(s);
            break ; 
          case 'A':
            s = strtok(NULL, ",");
            if (s != NULL) mot_a_conf.enable = atoi(s); 
            s = strtok(NULL, ",");
            if (s != NULL) mot_a_conf.dutyCycle = atoi(s);
            break ; 
          case 'B':
            s = strtok(NULL, ",");
            if (s != NULL) mot_b_conf.enable = atoi(s); 
            s = strtok(NULL, ",");
            if (s != NULL) mot_b_conf.dutyCycle = atoi(s);
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
            sprintf(buff, "R,S,%d,%d\r\n", spr_mot_conf.enable, spr_mot_conf.dutyCycle) ; 
            Serial.println(buff) ; 
            client.write(buff, strlen(buff));
            break ; 
          case 'A':
            sprintf(buff, "R,A,%d,%d,%d,%d,%d\r\n", mot_a_conf.enable, mot_a_conf.dutyCycle, enc_A_data.p_last, enc_A_data.p, enc_A_data.v) ; 
            client.write(buff, strlen(buff));
            break ; 
          case 'B':
            sprintf(buff, "R,B,%d,%d,%d,%d,%d\r\n", mot_b_conf.enable, mot_b_conf.dutyCycle, enc_B_data.p_last, enc_B_data.p, enc_B_data.v) ; 
            client.write(buff, strlen(buff));
            break ; 
          case 'I':
            sprintf(buff, "R,I,%d,%d", 1, 1) ; 
            client.write(buff, strlen(buff));
            break ; 
          case 'U':
            sprintf(buff, "R,U,%d",distanceCm) ; 
            client.write(buff, strlen(buff));
            break ; 
          default:
            break ;   
        }
        
      }
      else{
        Serial.println("Command not recognized") ;
        const char *buff = "Command not recognized\r\n";
        client.write(buff, strlen(buff));
      }
  }
}

void update_motors(){
  //motor a
  if(mot_a_conf.enable){
    if(mot_a_conf.dutyCycle > 0){
      ledcWrite(mot_a_channel_1, mot_a_conf.dutyCycle);
      ledcWrite(mot_a_channel_2, LOW);
    }else{
      ledcWrite(mot_a_channel_1, LOW);
      ledcWrite(mot_a_channel_2, -mot_a_conf.dutyCycle);
    }
  }else{
    ledcWrite(mot_a_channel_1, LOW);
    ledcWrite(mot_a_channel_2, LOW);
  }
  
  //motor b
  if(mot_b_conf.enable){
    if(mot_b_conf.dutyCycle > 0){
      ledcWrite(mot_b_channel_1, mot_b_conf.dutyCycle);
      ledcWrite(mot_b_channel_2, LOW);
    }else{
      ledcWrite(mot_b_channel_1, LOW);
      ledcWrite(mot_b_channel_2, -mot_b_conf.dutyCycle);
    }
  }else{
    ledcWrite(mot_b_channel_1, LOW);
    ledcWrite(mot_b_channel_2, LOW);
  }

  //spr motor
  if(spr_mot_conf.enable){
    spr_mot.write(spr_mot_conf.dutyCycle);
  }else{
    spr_mot.write(90);
  }
}

void update_encoder(){
  if (millis() > (prev_time + dt)){
    prev_time = millis();
    enc_A_data.p = encoder_A.getCount() / (75.81 * 6) * 2 * M_PI;
    enc_B_data.p = encoder_B.getCount() / (75.81 * 6) * 2 * M_PI;
    enc_A_data.v = (enc_A_data.p - enc_A_data.p_last) / (dt * 0.001);
    enc_B_data.v = (enc_B_data.p - enc_B_data.p_last) / (dt * 0.001);
    enc_A_data.p_last = enc_A_data.p;
    enc_B_data.p_last = enc_B_data.p;
  }
}

void read_ultrasonic(){
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  dT = pulseIn(ECHO_PIN, HIGH);
  distanceCm = dT * SOUND_SPEED/2;
}


void connect_to_WiFi(){
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
//===================================================================================================================================
