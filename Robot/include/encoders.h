/**********************************************************************************
 *  Encoder Object Header File
 * 
 * **********************************************************************************/

#include "Arduino.h"
#include "FunctionalInterrupt.h"


class Encoder{ 

public:
  int pinEncB;                    //Encoder pin B, pin A is only used in the interrupt.
  int side;                       //Encoder side, 0 for left, 1 for right
  volatile long odometer;         //total encoder counts
  volatile long speed;            //delta since last reading


  void EncoderISR();              //interrupt to read encoder counts accurately
  void Begin(int pinA, int pinB); //set up encoder pins and atatch ISR
  void Reset();                   //reset encoder counts
};

void Encoder::Begin(int pinA, int pinB){ 

  this->pinEncB = pinB; 
  this->odometer = 0;
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);

  attachInterrupt(digitalPinToInterrupt(pinA),std::bind(&Encoder::EncoderISR, this),RISING);
}


void Encoder::EncoderISR(){ //ISR to run every encoder tick

  volatile static int thisTime;
  volatile static int lastTime;
  volatile static int lastOdometer;

  // increment encoder reading
  if(digitalRead(pinEncB)) 
  {
  this->odometer += 1; // if pin B is high while pin A is high, motor is moving forwards
  }
  else
  {
  this->odometer -= 1; // if pin B is low, motor is moving backwards
  }

  // this line sets up a fast clock for each tick of the microcontroller
  asm volatile("esync; rsr %0,ccount":"=a" (thisTime)); // @ 240mHz clock each tick is ~4nS 

  if(thisTime - lastTime >= 25000000){ // every 100ms
    lastTime = thisTime;
    this->speed = this->odometer - lastOdometer;
    lastOdometer = this->odometer;
  }
}

void Encoder::Reset(){

  this->odometer = 0;
}