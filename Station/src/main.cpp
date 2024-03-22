#include <Wire.h>
#include <Arduino.h>
#include "Adafruit_TCS34725.h"
#include <SPI.h>
#include "ESP32Servo.h"

#define SDA_0 47
#define SCL_0 48
/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
//Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);
Servo s;


long starttime = millis();
long servodelay = 500;
long currenttime;
long servoStart = 0;
long tonesec; 
int son = 0;
void setup(void) {
  Wire.begin(48, 47);
  Serial.begin(9600);
  delay(2000);
  s.attach(38);
  s.write(90);
  if (tcs.begin()) {
    Serial.println("Found sense");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  // Now we're ready to get readings!
}

void loop(void) {
  currenttime = millis();
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  //colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  //lux = tcs.calculateLux(r, g, b);
  Serial.printf("r: %d g: %d  b: %d \n",r,g,b);

  if(r-g-b>8000 && son == 0){

    s.write(180);
    Serial.println("servo on");
    servoStart = currenttime;
    son = 1;
  }
  if (servoStart+servodelay<currenttime && son == 1){
    Serial.println("Servo off");
    s.write(90);
    son = 2;
  }
  if(servoStart+servodelay+servodelay<currenttime && son ==2){
      son = 0;
    }

  
  }