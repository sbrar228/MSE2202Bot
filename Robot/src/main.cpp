#include <Arduino.h>
#include "encoders.h"
#include "motors.h"

// put function declarations here:

#define LEFT_ENC_A          10                    // left encoder C1 on pin 10
#define LEFT_ENC_B          11                    // left encoder C2 on pin 11
#define RIGHT_ENC_A         12                    // right encoder C1 on pin 12
#define RIGHT_ENC_B         13                    // right encoder C2 on pin 13
#define LEFT_MOTOR_A        35                    // GPIO35 pin 28 (J35) Motor 1 A
#define LEFT_MOTOR_B        36                    // GPIO36 pin 29 (J36) Motor 1 B
#define RIGHT_MOTOR_A       37                    // GPIO37 pin 30 (J37) Motor 2 A
#define RIGHT_MOTOR_B       38                    // GPIO38 pin 31 (J38) Motor 2 B

#define STEPLENGTH          1000                   // length of one grid "tile"
#define ERRORMARGIN         10                     // margin of error in encoder counts



void updateTarget(char* instructionList, int step, int* target);

Encoder encoders[2];

Motor motors[2];

char path[21] = "ffffffffffbbbbbbbbbb"; // list of coordinates for the robot to follow:

// f: forwards one tile
// b: backwards one tile
// r: Turn 180 degrees to the right
// l: Turn 180 degrees to the left

int step = 0; //current position in the list of coordinates
bool targetReached = false;
int encoderTarget[2] = {0,0}; // target counts for left and right encoder
bool start = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);


  //setup Encoders
  encoders[0].Begin(LEFT_ENC_A,LEFT_ENC_B);
  encoders[1].Begin(RIGHT_ENC_B,RIGHT_ENC_A); // right is coded backwards so encoder shows correct direction
  //setup Motors
  motors[0].Begin(LEFT_MOTOR_A,LEFT_MOTOR_B,0,&encoders[0]);
  motors[1].Begin(RIGHT_MOTOR_A,RIGHT_MOTOR_B,1,&encoders[1]);
  updateTarget(path, step, encoderTarget);
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.printf(">Left:%d\n>Right:%d\n>Target_Left:%d\n>Target_Right:%d\n",encoders[0].odometer,encoders[1].odometer,encoderTarget[0],encoderTarget[1]);
  //Serial.printf("Left: %d | Right: %d || Target Left: %d | Target Right: %d || Target reached: %d || Left Gain: %f | Right Gain: %f \n",encoders[0].odometer,encoders[1].odometer,encoderTarget[0], encoderTarget[1], targetReached, motors[0].CalcGain(encoderTarget[0]),motors[1].CalcGain(encoderTarget[1]));
  delay(100);

  if(targetReached){

      step++;
      updateTarget(path, step, encoderTarget);

    if(path[step] == '\0'){
    while(1){
      motors[0].Move(0,1);
      motors[1].Move(0,1);
      Serial.print("Done!");
      delay(10000);
      step = 0;
      break;
    }
    }

    Serial.printf("New Target: %d, %d \n",encoderTarget[0],encoderTarget[1]);
    targetReached = false;

  } else{ 
    if(abs(encoders[0].odometer-encoderTarget[0])>=ERRORMARGIN){ //only move when error
      motors[0].MoveTarget(encoderTarget[0]);
    }else{motors[0].Stop();}
    if(abs(encoders[1].odometer-encoderTarget[1])>=ERRORMARGIN){
      motors[1].MoveTarget(encoderTarget[1]);
    } else{motors[0].Stop();}
  
    if(abs(encoders[0].odometer-encoderTarget[0])<ERRORMARGIN && abs(encoders[1].odometer-encoderTarget[1])<ERRORMARGIN){
      targetReached = true;
    }
  }
  
  
}

// put function definitions here:

void updateTarget(char* instructionList, int step, int* target){

  switch (instructionList[step]){
    case 'f':
      Serial.println("Set target forwards");
      target[0]+=STEPLENGTH;
      target[1]+=STEPLENGTH; 
      break;
    case 'b':
    Serial.println("Set target backwards");
      target[0]-=STEPLENGTH;
      target[1]-=STEPLENGTH;
      break;
  }

}