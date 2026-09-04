/* 
 Model RC plane Engine Dynamometer - Simone Redaelli, 18/04/2024 
 Modified from Nick Cinquino 9/1/15 - V.5 Original 10Kg loadcell
 Output data in grams (g). 
 Your calibration factor may be very positive or very negative. It all depends on the 
 setup of your individual loadcell, and the direction the sensors deflect from zero state. 
 Need to experiment! 
 Hack the HX711 board to set pin 15 high, for 80 samples per second. 
 This example code uses bogde's excellent library: https://github.com/bogde/HX711 
 bogde's library is released under a GNU GENERAL PUBLIC LICENSE 
 Arduino pin 2 - HX711 Clock 
 Arduino pin 3 - Serial Data In 
 5V - VCC 
 GND - GND 
 Most any pin on the Arduino Uno will be compatible with DOUT/CLK. 
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power is fine. 
 */
#include "HX711.h"   
#define DOUT 3 
#define CLK 2 
HX711 scale; 
char test = '0';
char temp = '0';  
float calibration_factor = -383.0755; //-383.0755 works for my 5kg loadcell. 
unsigned long start_time = 0;
unsigned long start_countdown = 0;
unsigned long countdown_time = 0;
int countdown = 0;
int j = 0;
int flag = 1;
int flag2 = 1;


void setup() { 
  Serial.begin(9600);
  scale.begin(DOUT, CLK);
  scale.set_scale(); 
  scale.tare(); //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading 
  Serial.print("\tZero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects. 
  Serial.println(zero_factor);
}

void loop() {
  scale.set_scale(calibration_factor); 
  if(flag) {
    Serial.println("RC Plane Dyno: press '1' to activate the test sequence, '2' to calibrate the setup"); //This is the menu that guides you in the configuration
    flag = 0;
    flag2 = 1;
  }

  while (!Serial.available()) { //Waiting for the user to enter a value
  }
  if(Serial.available() > 0) {
    test = Serial.read();
    switch (test) {
      case '2' :
        do {
          if(flag2) { //Manual configuration of the calibration factor
            Serial.print("Measured value:");
            Serial.println(scale.get_units(), 1);
            Serial.print("Calibration factor:");
            Serial.println(calibration_factor);
            Serial.println("Setup calibration: press '+' to increase the calibration factor, '-' to decrease it and 'e' to exit");
            flag2 = 0;
          }
          while (!Serial.available()) {
          }
          if(Serial.available()) {
            temp = Serial.read();
            switch (temp) {
              case '+':
                calibration_factor += 1;
                flag2 = 1;
                break;
              
              case '-':
                calibration_factor -= 1;
                flag2 = 1;
                break;
              
              case 'e':
                break;
              
              default:
                Serial.println("Invalid data entered!");
                break;
            }    
          }
        } while (temp != 'e');
        flag = 1;
        test = '0';
        break;

      case '1' :  //Sampling sequence 
        Serial.println("STAND CLEAR!"); 
        Serial.println("SAMPLING SEQUENCE ACTIVATED!");
        scale.set_scale(); 
        scale.tare(); 
        long zero_factor = scale.read_average(); 
        Serial.print("Zero factor: "); 
        Serial.println(zero_factor);
        Serial.println("The test starts in: 10s");
        start_countdown = millis();
        countdown_time = millis();
        while (millis() - start_countdown <= 10100) {
          if (millis() - countdown_time >= 1000) {
            j++;
            countdown = 10 - j;
            Serial.print("The test starts in: ");
            Serial.print(countdown);
            Serial.println("s");
            countdown_time = millis();
          }
        }
        Serial.println("AQUIRING DATA"); 
        start_time = millis();
        Serial.print("Start time, ms: ");
        Serial.println(start_time);
        while((millis() - start_time) <= 10100) {
          //for (int i=0; i <= 800; i++) { //800 samples at 80sa/sec = 10 seconds theoretical 
          scale.set_scale(calibration_factor); //Adjust to the calibration factor 
          Serial.print((millis() - start_time));
          Serial.print(": ");
          Serial.println(scale.get_units(), 1);
        }
        Serial.print("Stop Time, ms: "); 
        Serial.println(millis()); 
        Serial.println("SAMPLING SEQUENCE FINISHED!");
        flag = 1;
        test = '0';
        break; 

      default:
        Serial.println("Invalid data entered!");
        test = '0';
        break;
    }
  }
}