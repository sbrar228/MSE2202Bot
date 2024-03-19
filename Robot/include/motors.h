#include "Arduino.h"
#include <cmath>
class Motor{

  public:

  int pinMotorA;
  int pinMotorB;
  int side; // 0 for left, 1 for right
  Encoder* encoder; // Motor's Encoder

  void Begin(int pinA,int pinB, int side, Encoder* encoder); // Sets up motor pins and assigns motor side
  void Move(int dutyCycle, int direction); // Drives a single motor either backwards or forwards
  double CalcGain(int target); //returns Gain based on error 
  void MoveTarget(int target); //moves towards a set target
  void Stop(); //stops the motor
  
};

void Motor::Begin(int pinA,int pinB, int side, Encoder* encoder){

  //With this, left motor will have A and B on channels 1 and 2, right motor will have A and B on channels 3 and 4
  
  this->side = side;
  this->pinMotorA = pinA;
  this->pinMotorB = pinB;
  this->encoder = encoder;
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

// Moves the robot acccording to a set duty cycle
void Motor::Move(int dutyCycle, int direction){
  //direction 1 for forward, -1 for backwards

  if(direction == 1){ // if forwards
    if(this->side == 0){ // if left motor
      ledcWrite(1,0); //set left A Low, left B High to rotate clockwise
      ledcWrite(2,dutyCycle);
    }
    else{
      ledcWrite(3,dutyCycle); //set Right A High, left B Low to rotate counter-clockwise
      ledcWrite(4,0);
    }
  }
  if(direction == -1){
    if(this->side == 0){ // if left motor
      ledcWrite(1,dutyCycle); //set left A Low, left B High to rotate clockwise
      ledcWrite(2,0);
    }
    else{
      ledcWrite(3,0); //set Right A High, right B Low to rotate counter-clockwise
      ledcWrite(4,dutyCycle);
    }
  }
}

//Calculates the gain based on encoder position 
double Motor::CalcGain(int target){  

  double gain = (target-this->encoder->odometer);

  if (target==0){
    if(this->encoder->odometer<target) return 0.6; // to avoid divide by zero
    else return -0.6;
  }

  gain = gain/(abs(target));

  // constrain gain between 0.7 and 1
  if(gain>0){
    gain = 0.6+gain*0.4;
  }
  if(gain<0){
    gain = -0.6+gain*0.4;
  }
  if(gain>1.0){
    return 1.0;
  }
  if (gain<-1.0){
    return -1.0;
  }
  
  return gain;

}

void Motor::Stop(){

  if(this->side == 0){ // if left motor
      ledcWrite(1,0); //stop left motor
      ledcWrite(2,0);
    }
    else{
      ledcWrite(3,0); //stop right motor
      ledcWrite(4,0);
    }

}

void Motor::MoveTarget(int target){

  Move(abs(CalcGain(target))*255, (CalcGain(target)>0) ? 1 : -1 );

}


