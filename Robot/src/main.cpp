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

Encoder LeftEncoder;
Encoder RightEncoder;
Motor LeftMotor;
Motor RightMotor;

int i = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //setup Encoders
  LeftEncoder.Begin(LEFT_ENC_A,LEFT_ENC_B);
  RightEncoder.Begin(RIGHT_ENC_B,RIGHT_ENC_A); // right is coded backwards so encoder shows correct direction
  //setup Motors
  LeftMotor.Begin(LEFT_MOTOR_A,LEFT_MOTOR_B,0);
  RightMotor.Begin(RIGHT_MOTOR_A,RIGHT_MOTOR_B,1);
  
}
void loop() {
  // put your main code here, to run repeatedly:


  Serial.printf("Left: %d | Right: %d \n",LeftEncoder.odometer,RightEncoder.odometer);
  delay(100);
  LeftMotor.Move(255,1);
  RightMotor.Move(255,1);


}

// put function definitions here:
