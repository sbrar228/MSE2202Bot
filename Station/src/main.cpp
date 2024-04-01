#include <Wire.h>
#include <Arduino.h>
#include "Adafruit_TCS34725.h"
#include <SPI.h>
#include "ESP32Servo.h"
#include <WiFi.h>

#define SDA_0 47
#define SCL_0 48
#define SERVER_PORT 4080
#define LED_PIN  18 // ESP32 pin GPIO18 connected to LED ----- temporary

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

//wifi consts
const char* ssid = "suneetsiPhone";     // CHANGE TO YOUR WIFI SSID
const char* password = "20040531"; // CHANGE TO YOUR WIFI PASSWORD
WiFiServer TCPserver(SERVER_PORT);
WiFiClient client;

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

/////////////////////////////////////////////
///////////// begin wifi code setup /////////
////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  Serial.println("ESP32 #2: TCP SERVER + AN LED");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print your local IP address:
  Serial.print("ESP32 #2: TCP Server IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("ESP32 #2: -> Please update the serverAddress in ESP32 #1 code");

  // Start listening for a TCP client (from ESP32 #1)
  TCPserver.begin();
}
/////////////////////////////////////////////
///////////// end wifi code setup /////////
////////////////////////////////////////////

/////////////////////////////////////////////
///////////// begin wifi code loop /////////
////////////////////////////////////////////

void loop() {
  // If no client is connected, try to accept one
  if (!client.connected()) {
    client = TCPserver.available();
  } else {
    // Handle the connected client
    if (client.available()) {
      char command = client.read();
      Serial.print("ESP32 #2: - Received command: ");
      Serial.println(command);

      if (command == '1')
        digitalWrite(LED_PIN, HIGH); // Turn LED on   
         // ------------- change this part so that when the switches are pressed there's a delay for the robot to move back and the encoder moves the ramp
      else if (command == '0')
        digitalWrite(LED_PIN, LOW);  // Turn LED off
    }
  }
}

/////////////////////////////////////////////
///////////// end wifi code loop /////////
////////////////////////////////////////////


