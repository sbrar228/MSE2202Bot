/**********************************************************************************
 *  Program for the MSE Robot
 *  Written by Faris Amer, (insert name if you make any modifications)
 *  Overview:
 *  include/motors.h - header file for motor setup and movement
 *  include/encoders.h - header file for encoder position reading and setup
 *  src/main.cpp - main code
 *  
 *  Function explanations:
 * 
 *  updateTarget - accepts an instruction list and a step value, and then updates the positional target of both motors.
 * 
 *  This program works based on a step system. The path variable is a list of instrcutions, which gets parsed as the program runs.
 *  The file docs/map.png shows the currently programmed example path, which is the same as the demo video.
 *  
 *  Everything else should either be self explanatory, or have comments throughout explaining the work.
 * 
 * 
 * **********************************************************************************/




#include <Arduino.h>
#include "encoders.h"
#include "motors.h"
#include <ezButton.h>
#include <WiFi.h>

#define LEFT_ENC_A          10                    // left encoder C1 on pin 10
#define LEFT_ENC_B          11                    // left encoder C2 on pin 11
#define RIGHT_ENC_A         12                    // right encoder C1 on pin 12
#define RIGHT_ENC_B         13                    // right encoder C2 on pin 13
#define LEFT_MOTOR_A        35                    // GPIO35 pin 28 (J35) Motor 1 A
#define LEFT_MOTOR_B        36                    // GPIO36 pin 29 (J36) Motor 1 B
#define RIGHT_MOTOR_A       37                    // GPIO37 pin 30 (J37) Motor 2 A
#define RIGHT_MOTOR_B       38                    // GPIO38 pin 31 (J38) Motor 2 B

#define ROTATION          1160                    // encoder counts for one full rotation, 6cm wheel diameter, ~18.849 cm circumference
#define GRIDSIZE          20                       // size of one grid "step" in cm 
#define ERRORMARGIN       20                      // margin of error in encoder counts
#define WHEELBASE         16.5                    // wheelbase in cm
#define WHEEL_DIAMETER    6                       // wheel diameter in cm
#define BUTTON_PIN_1      21                      // ESP32 pin GPIO21 connected to button
#define BUTTON_PIN_2      22                      // ESP32 pin GPIO21 connected to button

//#define TELEPLOT                                // serial output formatted to be plotted on teleplot.fr, used for debugging/optimization





void updateTarget(char* instructionList, int step, int* target); //updates the target array according to the next step in the insructionList

//movement-related variables
Encoder encoders[2];            //array of encoder objects
Motor motors[2];                //array of motor objects
int encoderTarget[2] = {0,0};   // target counts for left and right encoder

char path[] = "ffrrrlffbt";     // list of coordinates for the robot to follow
/* f: forwards one tile
 * b: backwards one tile
 * r: Turn 90 degrees to the right (use even number of turns in a row to maintain 'grid' position)
 * l: Turn 90 degrees to the left
 * t: turn 180 degrees in place counterclockwise (used for unloading)
 */
int step = 0;                 //current position in the path
bool targetReached = false;   //to keep track of the state of step completion

// math constants
int countsPerCm = ROTATION/(WHEEL_DIAMETER*PI); // how many encoder counts per cm of travel
double stepsize = countsPerCm*GRIDSIZE;         // how many encoder counts per "step" in the path, based on  
double outerTurnRadius90 = (stepsize+WHEELBASE*countsPerCm)*PI/4; // encoder counts for outer wheel on a 90 degree turn
double innerTurnRadius90 = (stepsize-WHEELBASE*countsPerCm)*PI/4; // encoder counts for inner wheel on a 90 degree turn

///////////////
//wifi consts//
const char* ssid = "suneetsiPhone";     // CHANGE TO YOUR WIFI SSID
const char* password = "20040531"; // CHANGE TO YOUR WIFI PASSWORD
const char* serverAddress = "172.20.10.2"; // CHANGE TO ESP32#2'S IP ADDRESS
const int serverPort = 4080;
ezButton button1(BUTTON_PIN_1); // create ezButton for first button
ezButton button2(BUTTON_PIN_2); // create ezButton for second button
WiFiClient TCPclient;
bool bothButtonsPressed = false;
bool bothButtonsReleased = false;

void setup() {

  Serial.begin(115200);              
  //setup Encoders
  encoders[0].Begin(LEFT_ENC_A,LEFT_ENC_B);
  encoders[1].Begin(RIGHT_ENC_B,RIGHT_ENC_A); // right is coded backwards so encoder shows correct direction 
  //setup Motors
  motors[0].Begin(LEFT_MOTOR_A,LEFT_MOTOR_B,0,&encoders[0]);
  motors[1].Begin(RIGHT_MOTOR_A,RIGHT_MOTOR_B,1,&encoders[1]);
  updateTarget(path, step, encoderTarget); //set first target

/* ///////////////////////////////

    start of wifi setup code

*/////////////////////////////////
  
  button1.setDebounceTime(50); // set debounce time to 50 milliseconds
  button2.setDebounceTime(50); // set debounce time to 50 milliseconds
  Serial.println("ESP32: TCP CLIENT + A BUTTON/SWITCH");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // connect to TCP server (Arduino #2)
  if (TCPclient.connect(serverAddress, serverPort)) {
    Serial.println("Connected to TCP server");
  } else {
    Serial.println("Failed to connect to TCP server");
  }
  
  ///////////////////////
  // end of wifi setup //
  ///////////////////////
}

void loop() {


  #ifdef TELEPLOT
  // serial output formatted to be plotted on teleplot.fr, used for debugging/optimization
  Serial.printf(">Left:%d\n>Right:%d\n>Target_Left:%d\n>Target_Right:%d\n",encoders[0].odometer,encoders[1].odometer,encoderTarget[0],encoderTarget[1]);
  #endif

  if(targetReached){  // if target has been reached, update new target before proceeding

      step++;
      updateTarget(path, step, encoderTarget);

    if(path[step] == '\0'){ // if target has been reached and the new target is the last character in the string, stop the robot's movement.
    step = 0;
    while(1){
      motors[0].Move(0,1);
      motors[1].Move(0,1);
      Serial.print("Done!");
      delay(1000);
      //break;  //uncomment to loop program after finishing.
    }
    }

    Serial.printf("New Target: %d, %d \n",encoderTarget[0],encoderTarget[1]); // declare new target value
    targetReached = false; 

  } else{ // if the target has not been reached

    // if the error |current position - target position| is greater than the allowable value, keep running, else stop the wheel. calculated per wheel to avoid constant oscillation
    if(abs(encoders[0].odometer-encoderTarget[0])>=ERRORMARGIN){ 
      motors[0].MoveTarget(encoderTarget[0]);
    }else{motors[0].Stop();}

    if(abs(encoders[1].odometer-encoderTarget[1])>=ERRORMARGIN){
      motors[1].MoveTarget(encoderTarget[1]);
    } else{motors[1].Stop();}


    if(abs(encoders[0].odometer-encoderTarget[0])<ERRORMARGIN && abs(encoders[1].odometer-encoderTarget[1])<ERRORMARGIN){
      // if both wheels have error below the accepted value, the target has been reached.
      targetReached = true;
    }
  }
  
///////////////////////////////////////////////// 
//  
//           Start of wifi code                   
//                                    
/////////////////////////////////////////////////
  button1.loop(); // MUST call the loop() function first for button1
  button2.loop(); // MUST call the loop() function first for button2

  if (!TCPclient.connected()) {
    Serial.println("Connection is disconnected");
    TCPclient.stop();

    // reconnect to TCP server (Arduino #2)
    if (TCPclient.connect(serverAddress, serverPort)) {
      Serial.println("Reconnected to TCP server");
    } else {
      Serial.println("Failed to reconnect to TCP server");
    }
  }

  if (button1.isPressed() && button2.isPressed()) {
    bothButtonsPressed = true;
  }

  if (bothButtonsPressed){
    TCPclient.write('1');
    TCPclient.flush();
    Serial.println("- Both buttons are pressed, sent command: 1");
  }  

  if (button1.isReleased() && button2.isReleased() && bothButtonsPressed) {
    bothButtonsReleased = true;
  }

  if (bothButtonsReleased) {
    TCPclient.write('0');
    TCPclient.flush();
    Serial.println("- Both buttons are released, sent command: 0");
    bothButtonsReleased = false;
    bothButtonsPressed = false; // reset both buttons pressed state
  }

    /*/////////////////////////// 
        end of wifi code
    *////////////////////////////    
}

void updateTarget(char* instructionList, int step, int* target){

  switch (instructionList[step]){
    case 'f': // increase both wheels one step size
      target[0]+=stepsize;
      target[1]+=stepsize; 
      break;
    case 'b': // decrease both wheels one step size
      target[0]-=stepsize;
      target[1]-=stepsize;
      break;
    case 'r': // set targets for one 90 degree rotation right
      target[0]+= outerTurnRadius90;
      target[1]+= innerTurnRadius90;
      break;
    case 'l': // set targets for one 90 degree rotation left
      target[0]+= innerTurnRadius90;
      target[1]+= outerTurnRadius90;
      break;
    case 't': // rotate in place
      target[0]-= (WHEELBASE*countsPerCm)*PI/2;
      target[1]+= (WHEELBASE*countsPerCm)*PI/2;
      break;
  }

}
