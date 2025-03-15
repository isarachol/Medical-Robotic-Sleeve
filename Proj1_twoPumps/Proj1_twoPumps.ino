/******************************************************************************
ME 571 Project 1 Group 7 Spring 2025 Boston University
Modified from Lab 4 by Isara Cholaseuk and Tanish Katial

This code is for a medical robotic sleeve that responds to the amount of force
measured by force sensing resistors (FSRs). It has two pneumatic pumps used for 
haptic feedback which control pressures in the sleeve relative to the force exerted on FSRs.
The LED strip is attached to the sleeve to display relative force measured by FSRs.

Future improvements:
- Automatic calibration of control relationship between FSR values and pressure values
- Pull down resistors to reset MPRLS sensors (may help stabilize the I2C connections)
- Add more subsystems (for 2-3 more directions)
- Use PCB/soldering to clean wiring
- Create power supply system to power both arduino and circuits
- Reduce power consumptions from pumps/LEDs/solenoids
- Reduce time consumptions (reduce delays for all sensors; maybe a different board can help)
******************************************************************************/


/****************************** LIBREARIES *******************************/
#include <Wire.h>
#include <Adafruit_MPRLS.h>
#include "MapFloat.h"
#include <FastLED.h>

/****************************** INPUT *******************************/
// MPRLS pins and object
#define RESET_SENSOR_1 9
#define RESET_SENSOR_2 10
Adafruit_MPRLS mpr;

// Pump and Solenoid pins
const int pumpPin1 = 3;
const int pumpPin2 = 6;
const int solenoid1 = 4;
const int solenoid2 = 7;

// FSR analog pins
const int FSR_PIN1 = A0; // Pin connected to FSR/resistor divider
const int FSR_PIN2 = A2; // Pin connected to FSR/resistor divider

/****************************** OUTPUT *******************************/
// LED number, pin, and object
const int NUM_LEDS = 8;
const int LED_PIN = 2;
CRGB leds[NUM_LEDS];

/****************************** CONSTANTS *******************************/
// range const
// Try creating calibrating function next time
// Given by Prof. Russo and TAs
const float RangeLower = 0;
const float RangeUpper = 1.5;

/****************************** VARIABLES *******************************/
// Sensor variables
double Pmap1;
double Pmap2;
double Fmap1;
double Fmap2;

/****************************** SETUP *******************************/
void setup() {
  
  //Begin Serial Monitor ******************************************************
  Serial.begin(115200);
  Wire.begin();
  
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(200); // [0, 255]

  //Establish communication with MPRLS Pressure Sensor ************************
  pinMode(RESET_SENSOR_1, OUTPUT);
  pinMode(RESET_SENSOR_2, OUTPUT);

  // Start with both sensors in RESET mode
  digitalWrite(RESET_SENSOR_1, LOW);
  digitalWrite(RESET_SENSOR_2, LOW);
  
  delay(100); // Ensure sensors are fully reset

  Serial.println("Initializing sensors...");

  // Initialize Sensor 1
  digitalWrite(RESET_SENSOR_1, HIGH); // Activate Sensor 1
  delay(20); // Allow time to power up
  if (!mpr.begin()) {
      Serial.println("Failed to initialize MPRLS Sensor 1!");
      while (1) {
        errorLight();
      }
  }
  digitalWrite(RESET_SENSOR_1, LOW); // Reset Sensor 1

  // Initialize Sensor 2
  digitalWrite(RESET_SENSOR_2, HIGH); // Activate Sensor 2
  delay(20);
  if (!mpr.begin()) {
      Serial.println("Failed to initialize MPRLS Sensor 2!");
      while (1) {
        errorLight();
      }
  }
  digitalWrite(RESET_SENSOR_2, LOW); // Reset Sensor 2
  Serial.println("Found MPRLS sensor");
  
  //declare pin I/O ************************************************************
  pinMode(pumpPin1, OUTPUT);
  pinMode(pumpPin2, OUTPUT);
  pinMode(solenoid1,OUTPUT);
  pinMode(solenoid2,OUTPUT);

  //Initial deflate routine that will run once. Will depressurize any air that's
  //already in the line. 
  digitalWrite(solenoid1, LOW);   //open inlet valve 
  digitalWrite(solenoid2, LOW);   //open inlet valve 
  digitalWrite(pumpPin1, LOW);   //open inlet valve 
  digitalWrite(pumpPin2, LOW);   //open inlet valve 
  delay(1000);                         //wait 3 seconds for system to deflate

  initLight();
}

/****************************** LOOP *******************************/
void loop() {
//digitalWrite(solenoidInlet, LOW);   
//digitalWrite(solenoidExhaust, LOW);
//delay(10);
// NOTE:(Above) With better solenoids you can close both valves before reading the pressue in order to get a static pressure
// instead of dynamic. This will lead to much more accurate matching and less jumping.

  getForceValues();
  getPressureValues();

  //Printing force and pressure values
  Serial.print("Pressure1:") ; Serial.println(Pmap1);
  Serial.print(",");
  Serial.print("Force1:") ; Serial.println(Fmap1);
  Serial.print("Pressure2:") ; Serial.println(Pmap2);
  Serial.print(",");
  Serial.print("Force2:") ; Serial.println(Fmap2);
  Serial.println();

  setPumpControl();
  //Add LED controls to loop
  forceLightUpdate(2*(Fmap1+Fmap2)); //use average force
}

/****************************** FUNCTIONS *******************************/
// turn on - off pumps to regulate sleeve pressures depending on conatct force
void setPumpControl() {
  //simple control loop, if Force > pressure, glove is inflated. If Force < pressure, glove is deflated
  // Fmap1 and Pmap1 controls pump and solenoid 1
  if (Fmap1 > Pmap1) {
    digitalWrite(solenoid1, HIGH);
    digitalWrite(pumpPin1, HIGH);
  }
  else { // if pressure is too high, stop the pump
   digitalWrite(solenoid1, LOW);
   digitalWrite(pumpPin1, LOW); 
  }
  delay(10);
  // Fmap2 and Pmap2 controls pump and solenoid 2
  if (Fmap2 > Pmap2) {
    digitalWrite(solenoid2, HIGH);
    digitalWrite(pumpPin2, HIGH);
  }
  else { // if pressure is too high, stop the pump
   digitalWrite(solenoid2, LOW);
   digitalWrite(pumpPin2, LOW); 
  }
  delay(10);
}

// read force value from the two FSR and map it
void getForceValues() {
  float forceVal1 = analogRead(FSR_PIN1); //reading force pin
  forceVal1 = analogRead(FSR_PIN1); //reading force pin
  delay(2);
  float forceVal2 = analogRead(FSR_PIN2);
  forceVal2 = analogRead(FSR_PIN2);
  delay(2);
  
  // max value goes upto 800 (out of 1023)
  // This value depends on the voltage divider circuit --> change resistance there to adjust the value
  Fmap1 = mapFloat(forceVal1, 0, 800, RangeLower,RangeUpper);
  delay(2);
  Fmap2 = mapFloat(forceVal2, 0, 800, RangeLower,RangeUpper);
  delay(10);
}

// read pressure values from MRLPS 1 and 2 and map it to the range
// Problem: MPRLS sensors have the same I2C address, so one of them needs to be reset
// Try pull down resistors nest time
void getPressureValues() {
  // get Pmap1
  digitalWrite(RESET_SENSOR_2, LOW); // Reset Sensor 2
  digitalWrite(RESET_SENSOR_1, HIGH); // Activate Sensor 1
  delay(50); // Allow time for the sensor to wake up
  //Serial.println("reset mprls1 reached");
  float pressure_Psi = mpr.readPressure()/68.947572932; //reading pressure in PSI
  pressure_Psi = mpr.readPressure()/68.947572932; //reading pressure in PSI
  delay(2);
  Pmap1 = mapFloat(pressure_Psi,14.80,15,RangeLower,RangeUpper); // mapping force and pressure to the same range
  delay(2);

  // get Pmap2
  digitalWrite(RESET_SENSOR_1, LOW); // Reset Sensor 1
  digitalWrite(RESET_SENSOR_2, HIGH); // Activate Sensor 2
  delay(50); // Allow time for the sensor to wake up
  //Serial.println("reset mprls1 reached");
  pressure_Psi = mpr.readPressure()/68.947572932; //reading pressure in PSI
  pressure_Psi = mpr.readPressure()/68.947572932; //reading pressure in PSI
  delay(2);
  Pmap2 = mapFloat(pressure_Psi,14.83,15,RangeLower,RangeUpper); // mapping force and pressure to the same range
  delay(2);

  // When the pressure is too low (incorrect reading) and pump is activated, fix that value
  if (Pmap1 < RangeLower)
    Pmap1 = RangeLower;
  if (Pmap2 < RangeLower)
    Pmap2 = RangeLower;
}


// Update light linearly corresponding to the mapped force [RangeLower, RangeUpper]
// EX. there are 8 LEDs; Force is 7.5/8 --> turn on 7 LEDs and the 8th is turned on 50%
void forceLightUpdate(float force) {
  float scale = (force - RangeLower)/(RangeUpper - RangeLower)*NUM_LEDS; 
  int num_leds_full = floor(scale); // number of leds fully turning on
  float last_led_vibrance = round((scale - num_leds_full)*255); // partial brightness

  if (scale >= NUM_LEDS-0.1) { // if the force is at maximum
    for (int j=NUM_LEDS-1; j>-1; j--){ // start from the front of the hand
      setColor(j, 255, 0, 0); //red
    }
  }
  else { //if force is not max
    for (int j=NUM_LEDS-1; j>-1; j--){
      if (j>NUM_LEDS-num_leds_full-1){
        setColor(j, 0, 255, 0); //green
      }
      else if (j == NUM_LEDS-num_leds_full-1){ // partial
        setColor(j, 0, last_led_vibrance,0); // hopefully less bright or something
      }
      else {
        setColor(j, 0, 0, 0); // black
      }
    }
  }

  FastLED.show(); // send the signal
  delay(20);
}

// Flash blue light to indicate that the setup step is done
void initLight() {
  for (int j=0; j<NUM_LEDS; j++){ // start from the front of the hand
    setColor(j, 0, 0, 255); //blue
  }

  FastLED.show(); // send the signal
  delay(1000);

  for (int j=0; j<NUM_LEDS; j++){ // start from the front of the hand
    setColor(j, 0, 0, 0); //off
  }

  FastLED.show(); // send the signal
  delay(100);
}

// Flashing red light when MPRLS sensors are not found by arduino --> need to reset arduino
void errorLight() {
  for (int j=0; j<NUM_LEDS; j++){ // start from the front of the hand
    setColor(j, 255, 0, 0); //red
  }

  FastLED.show(); // send the signal
  delay(200);

  for (int j=0; j<NUM_LEDS; j++){ // start from the front of the hand
    setColor(j, 0, 0, 0); //off
  }

  FastLED.show(); // send the signal
  delay(100);
}

// Setting color given the index of led (the i-th led), r, g, b values
// r,g,b in byte [0,255]
void setColor(int led_num, int r, int g, int b) {
  leds[led_num].r = r;
  leds[led_num].g = g;
  leds[led_num].b = b;
}
