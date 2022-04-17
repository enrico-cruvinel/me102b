#include <Arduino.h>
#include <ESP32Encoder.h>

#define SPR_MOT 26
#define MOT_A_1 26
#define MOT_A_2 25
#define ENC_A_1 4
#define ENC_A_2 5
#define MOT_B_1 34
#define MOT_B_2 39
#define ENC_B_1 13
#define ENC_B_2 12
#define ECHO_PIN
#define TRIG_PIN


/******* functions *******/ 
void decode_message() ; 

/**** data structures ****/
typedef struct encoder_t ; 
typedef struct motor_conf_t ;
typedef struct motor_data_t ;

/******** motors ********/
const int freq = 5000;
const int resolution = 8;
int dutyCycle = 0 ; 

const int mot_a_channel_1 = 1;
const int mot_a_channel_2 = 2;
const int mot_b_channel_1 = 3;
const int mot_b_channel_2 = 4;

int spr_mot_en = 0 ;
int mot_a_en = 0 ; 
int mot_b_en = 0 ; 

int count=0;
unsigned long on_time=0;
unsigned long start_time, now = 0 ;
/***********************/

/******** encoders ********/
ESP32Encoder encoder_A;
ESP32Encoder encoder_B;
float p_A = 0;
float p_B = 0;
float v_A = 0;
float v_B = 0;
float p_A_last = 0;
float p_B_last = 0;
unsigned long prev_time = 0;
const long dt = 20;


//=====================================================================================================================================
void setup() 
{

  /*** motors ***/
  //pwm channel
  ledcSetup(spr_mot_channel, freq, resolution);
  ledcSetup(mot_a_channel_1, freq, resolution);
  ledcSetup(mot_a_channel_2, freq, resolution);
  ledcSetup(mot_b_channel_1, freq, resolution);
  ledcSetup(mot_b_channel_2, freq, resolution);
  //pins
  ledcAttachPin(SPR_MOT, spr_mot_channel); 
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

  Serial.begin(115200);

}

//=====================================================================================================================================
void loop() {

  /************ read message *************/

  /************************************/

  /*********** motor write ************/
//  if ((millis() - start_time) < on_time){
//    ledcWrite(ledChannel, dutyCycle);
//  } else{
//    ledcWrite(ledChannel, 0);
//    on_time = 0 ;  
//  }
  /************************************/
    
  /************* encoder **************/
  if (millis() > (prev_time + dt)){
    prev_time = millis();
    p_A = encoder_A.getCount() / (75.81 * 6) * 2 * M_PI;
    p_B = encoder_B.getCount() / (75.81 * 6) * 2 * M_PI;
    v_A = (p_A - p_A_last) / (dt * 0.001);
    v_B = (p_B - p_B_last) / (dt * 0.001);
    p_A_last = p_A;
    p_B_last = p_B;
  }
  
  /***********************************/
      
}


/************************************/
/*          helper functions        */
/************************************/

void decode_message(){
  if(Serial.available()>0){
    
    // read data
    char server_message[100] ; 
    byte buffSize = Serial.readBytesUntil('\n', server_message, 100);
    server_message[buffSize] = 0 ; 
    Serial.println(server_message);

    // check cmd. S = spring motor, A = motor A, B = motor B
    if(server_message[0]) {
      char *s = strtok(server_message, ","); //allows string to be broken into tokens by " ".
      s = strtok(NULL, ",");
      case "R"
      case "M":
      case "A":
      case "B":
      case ""
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
  }
}

/************************************/
/*          type definitions        */
/************************************/
typedef struct motor_conf_t{
  const int freq = 5000;
  const int resolution = 8;
  const int mot_channel = 0;
  int dutyCycle = 0; 
  int enable = 0;
} motor_conf_t ; 

typedef struct encoder_data_t{
  float p = 0;
  float v = 0;
  float p_last = 0;
} encoder_data_t ; 

//===================================================================================================================================
