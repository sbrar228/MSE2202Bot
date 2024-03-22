/**********************************************************************************
 *  Motor Object Header File
 * 
 * **********************************************************************************/

#include "Arduino.h"
#include <cmath>


class Motor{

  public:
  int pinMotorA;    //Motor driver pin A connected to the motor
  int pinMotorB;    //Motor driver pin B connected to the motor
  int side;         //to keep track of the side of the motor, 0 for left, 1 for right
  Encoder* encoder; //pointer to encoder object associated with the motor


  void Begin(int pinA,int pinB, int side, Encoder* encoder);      //Sets up motor pins and assigns motor side, as well as setting up ledc channels for each motor
  void Move(int dutyCycle, int direction);                        //Drives a single motor either backwards or forwards at a set power via PWM
  double CalcGain(int target);                                    //returns gain based on error by using a proportional function
  void MoveTarget(int target);                                    //moves towards a set target using the proportional gain
  void Stop();                                                    //stops the motor
  
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


void Motor::Move(int dutyCycle, int direction){
  //direction 1 for forward, -1 for backwards

  if(direction == 1){         // if forwards
    if(this->side == 0){      // if left motor
      ledcWrite(1,0);         //set left A Low, left B High to rotate clockwise
      ledcWrite(2,dutyCycle);
    }
    else{
      ledcWrite(3,dutyCycle); //set Right A High, left B Low to rotate counter-clockwise
      ledcWrite(4,0);
    }
  }
  if(direction == -1){
    if(this->side == 0){      // if left motor
      ledcWrite(1,dutyCycle); //set left A Low, left B High to rotate clockwise
      ledcWrite(2,0);
    }
    else{
      ledcWrite(3,0);         //set Right A High, right B Low to rotate counter-clockwise
      ledcWrite(4,dutyCycle);
    }
  }
}

//Calculates the gain based on encoder position using a proportional gain function
//This function isn't perfect and probably will not work if it is scaled to 

double Motor::CalcGain(int target){  

  double gain = (target-this->encoder->odometer);

  if (target==0){ // to avoid dividing by zero, and to slow down when resetting.
    if(this->encoder->odometer<target) return 0.7; 
    else return -0.7;
  }

  gain = gain/(abs(target));

  // constrain gain between 0.7 and 1, and linear ramp between the two values
  if(gain>0){
    gain = 0.7+gain*0.3;
  }
  if(gain<0){
    gain = -0.7+gain*0.3;
  }
  if(gain>0.9){
    return 0.9;
  }
  if (gain<-0.9){
    return -0.9;
  }
  
  return gain;

}

void Motor::Stop(){ // stops the motor

  if(this->side == 0){ // if left motor
      ledcWrite(1,0); //stop left motor
      ledcWrite(2,0);
    }
    else{
      ledcWrite(3,0); //stop right motor
      ledcWrite(4,0);
    }

}

void Motor::MoveTarget(int target){ // takes a target value, calculates the gain, then sets the speed and direction according to the gain returned.

  Move(abs(CalcGain(target))*255, (CalcGain(target)>0) ? 1 : -1 );

}


