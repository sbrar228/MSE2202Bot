#include "Arduino.h"

class Motor{

  public:

  int pinMotorA;
  int pinMotorB;
  int side; // 0 for left, 1 for right
  void Begin(int pinA,int pinB, int side); // Sets up motor pins and assigns motor side
  void Move(int dutyCycle, int direction); // Drives a single motor either backwards or forwards
};

void Motor::Begin(int pinA,int pinB, int side){

  //With this, left motor will have A and B on channels 1 and 2, right motor will have A and B on channels 3 and 4

  this->side = side;
  this->pinMotorA = pinA;
  this->pinMotorB = pinB;

  if(side == 0){
    ledcAttachPin(pinA,1);
    ledcSetup(1, 20000, 8);
    ledcAttachPin(pinB,2);
    ledcSetup(2,20000, 8);
  }
  else{
    ledcAttachPin(pinA,3);
    ledcSetup(3, 20000, 8);
    ledcAttachPin(pinB,4);
    ledcSetup(4,20000, 8);
  }
}

void Motor::Move(int dutyCycle, int direction){
  //direction 1 for forward, -1 for backwards

  if(direction = 1){ // if forwards
    if(this->side == 0){ // if left motor
      ledcWrite(1,0); //set left A Low, left B High to rotate clockwise
      ledcWrite(2,dutyCycle);
    }
    else{
      ledcWrite(3,dutyCycle); //set Right A High, left B Low to rotate counter-clockwise
      ledcWrite(4,0);
    }
  }
  else{
    if(this->side == 0){ // if left motor
      ledcWrite(1,dutyCycle); //set left A Low, left B High to rotate clockwise
      ledcWrite(2,0);
    }
    else{
      ledcWrite(3,0); //set Right A High, left B Low to rotate counter-clockwise
      ledcWrite(4,dutyCycle);
    }
  }

}