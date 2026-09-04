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

 Wiring: 
 The HX711 board can be powered from 2.7V to 5V so the Arduino 5V power is fine.
 - Arduino pin 2 -> HX711 Clock for thrust
 - Arduino pin 3 -> HX711 Data for thrust
 - Arduino pin 4 -> HX711 Clock for torque
 - Arduino pin 10 -> HX711 Data for torque
 - LCD connected to digital pins 5-12
 - LEDs connected to pins 1 and 13
 - Buttons for control connected to A0, A1, A2
 Most any pin on the Arduino Uno will be compatible with DOUT/CLK. 
 REFER TO THE SCHEMATIC IN THE FOLDER
*/ 

#include "HX711.h"   //Include the HX711 library for load cell reading
#include <LiquidCrystal.h> //Include the LCD display library

//Define variables for the script
#define TIMER 3

//Define the pins for thrust measurement
#define DOUT_TH 3 
#define CLK_TH 2 

//Define the pins for torque measurement
#define DOUT_TO 10
#define CLK_TO  4

//Define the pin for the LCD display V0 pin (contrast adjustment)
#define V0 9

//Define the LED pins
#define GREEN_LED 1
#define RED_LED 13

//Define button pins
#define on_off_button A2
#define increase_button A1
#define decrease_button A0

//Create instances for load cells
HX711 th_scale; //Thrust measurement
HX711 to_scale; //Torque meseaurement

//Define LCD pins and initialize the LCD
const int rs = 12, en = 11, d4 = 8, d5 = 7, d6 = 6, d7 = 5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); //vss to ground, vdd to +5V, RW to GND, V0 to PIN 9, A to +5V and K top GND

//Set LCD contrast
int contrast = 90;

//Variables for calibration and operation
volatile char test = '0';
char temp = '0';  
float calibration_factor_th = -98.16;  //Initial calibration factor for thrust. -82.08 works for my 20kg loadcell as a starting point, then manual calibrate the cell. 
float calibration_factor_to = -100.08;  //Initial calibration factor for torque. -56.08 works for my 20kg loadcell as a starting point, then manual calibrate the cell. 

//Timing variables
unsigned long start_time = 0;
unsigned long start_countdown = 0;
unsigned long countdown_time = 0;
unsigned long print_time = 0;
unsigned long start_lcd_time = 0;
unsigned long print_lcd_time = 0;

//Index variables
int countdown = 0;
int j = 0;

//Flag variables
int flag = 1;
int flag2 = 1;
int flag3 = 0;
int flag4 = 1;


// --- FUNZIONE SPECIALE PER SCARICARE I PIN FLOTTANTI ---
void discharge_pin(int pin) {
  pinMode(pin, OUTPUT);      // Trasforma pin in uscita
  digitalWrite(pin, LOW);    // Porta a 0V (GND) per scaricare la capacità
  delay(5);                  // Aspetta che si scarichi
  pinMode(pin, INPUT);       // Riporta in lettura
}


void setup() {
  //Initialize pins
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(on_off_button, INPUT);
  pinMode(increase_button, INPUT);
  pinMode(decrease_button, INPUT);

   //Configure LCD contrast
  pinMode(V0, OUTPUT);
  analogWrite(V0, contrast);

  //Initialize LCD
  lcd.begin(16, 2);

  //Initialize serial communication 
  Serial.begin(9600);

  Serial.println("---SETUP---");
  //Initialize thrust sensor         
  th_scale.begin(DOUT_TH, CLK_TH);
  th_scale.set_scale(); 
  th_scale.tare(); //Reset the scale to 0 for thrust
  long zero_factor_th = th_scale.read_average(); //Get a baseline reading for thrust
  Serial.print("Zero factor for thrust: "); //This can be used to remove the need to tare the scale for thrust. Useful in permanent scale projects. 
  Serial.println(zero_factor_th);

  //Initialize torque sensor
  to_scale.begin(DOUT_TO, CLK_TO);
  to_scale.set_scale(); 
  to_scale.tare(); //Reset the scale to 0 for torque
  long zero_factor_to = to_scale.read_average(); //Get a baseline reading for torque
  Serial.print("Zero factor for torque: "); //This can be used to remove the need to tare the scale for torque. Useful in permanent scale projects. 
  Serial.println(zero_factor_to);

  Serial.println("Setup completed!\nWait for the program to start.\n");
}

void loop() {
  //Set scale factors
  th_scale.set_scale(calibration_factor_th);
  to_scale.set_scale(calibration_factor_to);

  //Display initial LCD message for 8 seconds
  while (millis() - start_lcd_time <= 8000) {
    if (millis() - print_lcd_time >= 500) {
      lcd.clear(); 
      lcd.setCursor(0,0); 
      lcd.print("RC Plane Dyno"); 
      lcd.setCursor(0,1); 
      lcd.print("(Sampling on PC)");
      print_lcd_time = millis();
    }  
  }

  //Warning message for 3 seconds
  while (millis() - start_lcd_time > 8000 && millis() - start_lcd_time <= 11000) {
    if (millis() - print_lcd_time >= 500) {
      lcd.clear(); 
      lcd.setCursor(0,0); 
      lcd.print("STAND CLEAR!");
      lcd.setCursor(0,1); 
      lcd.print("BE CAREFUL!");
      print_lcd_time = millis();
    }  
  }    
  
  //Display menu options on Serial Monitor
  if(flag) {
    //This is the menu that guides you in the configuration
    Serial.println("---RC PLANE DYNO---");
    Serial.println("Press '1' to activate the test sequence");
    Serial.println("Press '2' to calibrate the thrust's calibration factor");
    Serial.println("Press '3' to calibrate the torque's calibration factor");
    Serial.println("Press 'e' to terminate the program");
    flag = 0;
    flag2 = 1;
  }

  //Display on the LCD display thrust/torque readings until user input for sampling
  while (!Serial.available()) { //Waiting for the user to enter a value meanwhile the thrust values are displayed on the LCD display for fast measurements
    //Serial.print(digitalRead(on_off_button));
    if (millis() - print_time >= 1000) {
      th_scale.set_scale(calibration_factor_th);
      to_scale.set_scale(calibration_factor_to);
      lcd.clear(); 
      lcd.setCursor(0,0); 
      lcd.print("Thr [N]:  ");
      //lcd.print((th_scale.get_units()), 1); //valore misurato in grammi
      lcd.print((0.00981*th_scale.get_units()), 1);
      lcd.setCursor(0,1);
      lcd.print("Trq [Nm]: ");
      lcd.print((0.000981*to_scale.get_units()), 3);
      print_time = millis();
    }

    //LED status: system on, acquisition mode off
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

  if(digitalRead(on_off_button) == HIGH) {
    //Rising edge detection
    while(digitalRead(on_off_button) == HIGH) {
      discharge_pin(on_off_button);
    }
    //LED status: calibration of the cells
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, HIGH);

    //Manual calibration using buttons
    flag3 = 1;

    // --- SETUP THRUST ---
    while (flag3 == 1) {
      //For thrust
      if (millis() - print_time >= 500) {
      lcd.clear(); 
      lcd.setCursor(0,0); 
      lcd.print("Setup-th:");
      th_scale.set_scale(calibration_factor_th);
      lcd.print(th_scale.get_units(), 1);
      lcd.print("g"); 
      lcd.setCursor(0,1); 
      lcd.print("C.f.th.:"); 
      lcd.print(calibration_factor_th);
      print_time = millis();
      }

      calibration_factor_th = manual_calibration(calibration_factor_th);

      if(digitalRead(on_off_button) == HIGH) {
        //Rising edge detection
        while(digitalRead(on_off_button) == HIGH) { 
          discharge_pin(on_off_button);
        }
        flag3 = 2;
      }
    }

    while (flag3 == 2) {
      //For torque
      if (millis() - print_time >= 500) {
      lcd.clear(); 
      lcd.setCursor(0,0); 
      lcd.print("Setup-to:");
      to_scale.set_scale(calibration_factor_to);
      lcd.print(0.000981*to_scale.get_units(), 1);
      lcd.print("Nm"); 
      lcd.setCursor(0,1); 
      lcd.print("C.f.to.:"); 
      lcd.print(calibration_factor_to);
      print_time = millis();
      }
      
      calibration_factor_to = manual_calibration(calibration_factor_to);
      
      
      if(digitalRead(on_off_button) == HIGH) {
        //Rising edge detection
        while(digitalRead(on_off_button) == HIGH) { 
          discharge_pin(on_off_button);
        }
        flag3 = 0;
      }
    }
  }

  }
  if(Serial.available() > 0) {
    lcd.clear(); 
    lcd.setCursor(0,0);
    test = Serial.read();
    if (test == '1') {
      lcd.print("ACQUISITION MODE");
    }
    if (test == '2' || test == '3') {
      lcd.print("SETUP MODE");
    }
    lcd.setCursor(0,1); 
    lcd.print("LOOK TO THE PC"); 
    //Clears the serial buffer
    while (Serial.available() > 0) {
      Serial.read();  //Reads and discards all remaining data in the buffer
    }
  
    switch (test) {
      default:
        Serial.println("Invalid data entered!");
        test = '0';
        break;

      //Manual calibration using PC
      //For thrust
      case '2' :
        //LED status: calibration of the cells
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, HIGH);
        calibration_factor_th = calibration(calibration_factor_th, "thrust");
        break;
      //For torque
      case '3' :
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, HIGH);
        calibration_factor_to = calibration(calibration_factor_to, "torque");
        break;
      
      case '1' :  //Sampling sequence
        // Start sampling sequence
        digitalWrite(GREEN_LED, LOW);
        digitalWrite(RED_LED, HIGH);
        Serial.println("\nSTAND CLEAR! SAMPLING SEQUENCE ACTIVATED!");

        //Reset scales
        th_scale.set_scale(); 
        th_scale.tare();
        to_scale.set_scale(); 
        to_scale.tare();  
        long zero_factor_th = th_scale.read_average(); 
        Serial.print("Zero factor for thrust: "); 
        Serial.println(zero_factor_th);
        long zero_factor_to = to_scale.read_average(); 
        Serial.print("Zero factor for torque: "); 
        Serial.println(zero_factor_to);
        Serial.println("");

        while (Serial.available() > 0) {
          Serial.read();  //Reads and discards all remaining data in the buffer
        }

        Serial.println("---DATA ACQUISITION---");
        Serial.println("Template -> time[ms]: thrust[N]  torque[Nm]");
        Serial.println("Press Spacebar to stop the acquisition!\n");

        //Countdown before test starts
        Serial.print("The test starts in: ");
        Serial.print(TIMER);
        Serial.println("s");
        start_countdown = millis();
        countdown_time = millis();
        while (millis() - start_countdown <= TIMER*1000 + 100) {
          if (millis() - countdown_time >= 1000) {
            j++;
            countdown = TIMER - j;
            Serial.print("The test starts in: ");
            Serial.print(countdown);
            Serial.println("s");
            countdown_time = millis();
          }
        }
        j = 0;
        Serial.println("");
        
        //Start data acquisition
        flag4 = 1;
        start_time = millis();
        while(flag4) {
          while (Serial.available() > 0) {
            if (Serial.read() == ' ') {
              flag4 = 0;
            }
          }
          th_scale.set_scale(calibration_factor_th); //Adjust to the calibration factor for thrust
          to_scale.set_scale(calibration_factor_to); //Adjust to the calibration factor for torque
          Serial.print((millis() - start_time));
          Serial.print(": ");
          Serial.print((0.00981*th_scale.get_units()), 3);
          Serial.print("\t");
          Serial.println((0.000981*to_scale.get_units()), 4);
        }
        Serial.println("\nSAMPLING SEQUENCE FINISHED!\n");
        flag = 1;
        test = '0';
        break; 

    }
  }
}

//Function for manual calibration using buttons
float manual_calibration(float calibration_factor) {
  if(digitalRead(increase_button) == HIGH) {
    //Rising edge detection
    while(digitalRead(increase_button) == HIGH) { 
      discharge_pin(increase_button);
    }
    calibration_factor = calibration_factor + 1;
  }
  if(digitalRead(decrease_button) == HIGH) {
    while(digitalRead(decrease_button) == HIGH) {
      discharge_pin(decrease_button);  
    }
    calibration_factor = calibration_factor - 1;
  }
  return calibration_factor;
}

//Function for manual calibration using PC
float calibration(float calibration_factor, String measurand) {
  do {
          if(flag2) { 
            Serial.print("Measured value of " + measurand + ": ");
            if (measurand.equals("thrust")) {
              th_scale.set_scale(calibration_factor);
              Serial.print(th_scale.get_units(), 1);
            }
            if (measurand.equals("torque")) {
              to_scale.set_scale(calibration_factor);
              Serial.print(to_scale.get_units(), 1);
            }
            Serial.println("g");
            Serial.print("Calibration factor for the " + measurand + ": ");
            Serial.println(calibration_factor);
            Serial.println("Setup calibration: press '+' to increase the calibration factor for " + measurand + ", '-' to decrease it and 's' to exit");
            flag2 = 0;
          }
          while (!Serial.available()) {
          }
          if(Serial.available() > 0) {
            temp = Serial.read();
            //Clears the serial buffer
            unsigned long start_time_cl_buffer = millis();  //Start time for buffer cleaning (wait for the other letters)
            while (millis() - start_time_cl_buffer < 10) { // Wait 10ms without blocking code execution
              //Clears the serial buffer
              if (Serial.available() > 0) {
                Serial.read();  //Reads and discards all remaining data in the buffer
              }
            }

            switch (temp) {
              case '+':
                calibration_factor += 1;
                flag2 = 1;
                break;
              
              case '-':
                calibration_factor -= 1;
                flag2 = 1;
                break;
              
              case 's':
                break;
              
              default:
                Serial.println("Invalid data entered!");
                break;
            }    
          }
        } while (temp != 's');
        flag = 1;
        test = '0';
        return calibration_factor;
}