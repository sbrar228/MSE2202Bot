/********************************************************
Station Code


PINOUT:
SDA: 21
SCL: 22

SERVO: 19
MOTOR+: 2
MOTOR-:15
*********************************************************/

#include <Wire.h>
#include <Arduino.h>
#include "Adafruit_TCS34725.h"
#include <SPI.h>
#include "ESP32Servo.h"
#include "../../Robot/include/motors.h"

#define MOTOR_A 2
#define MOTOR_B 15
#define ENCODER_A 33
#define ENCODER_B 32

//Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);
Servo s;
Motor pulley;
Encoder pEnc;

long starttime = millis();
long servodelay = 1000;
long currenttime;
long servoStart = 0;
long tonesec; 
int son = 0;
int pulleyTarget;
String incomingByte;
bool up;

void setup(void) {

  Serial.begin(9600);
  s.attach(19);

  //pEnc.Begin(ENCODER_A,ENCODER_B);
  //pulley.Begin(MOTOR_A,MOTOR_B,0,&pEnc);

  if (tcs.begin()) {
    Serial.println("Found sense");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
  }
  up = true;

}

void loop(void) {


  currenttime = millis();
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);

  Serial.printf("r: %d g: %d  b: %d \n",r,g,b);
  
  if(g-r>400 && g-b>400 && son == 0){

    s.write(105);
    Serial.println("servo 1");
    servoStart = currenttime;
    son = 1;

  }
  else if(son == 0){
    s.write(75);
    Serial.println("servo 1");
    servoStart = currenttime;
    son = 1;
  }
  if (servoStart+servodelay<currenttime && son == 1){
    Serial.println("Servo 2");
    s.write(90);
    son = 2;
  }
  if(servoStart+servodelay+servodelay<currenttime && son ==2){
      Serial.println("Servo 0");
      son = 0;
    }

  if(up){
    pulleyTarget = 1000;
  } else{
    pulleyTarget = 0;
  }
  if(abs(pulley.encoder->odometer - pulleyTarget) >50){
  pulley.MoveTarget(pulleyTarget);
  } else{
    pulley.Stop();
  }
  
  

}
