/*
 * This ESP8266 NodeMCU code was developed by newbiely.com
 *
 * This ESP8266 NodeMCU code is made available for public use without any restriction
 *
 * For comprehensive instructions and wiring diagrams, please visit:
 * https://newbiely.com/tutorials/esp8266/esp8266-keypad-relay
 */

/**
 * @file main.cpp
 * @brief This is the main file for the ESP32-WROOM-32 code
 * @author Ben Himmel and newbiely.com
 * @date 2024-01-28
 * @version 1.3
 */

// Include the required libraries
#include <Arduino.h>
#include <Keypad.h>

#define ROW_NUM            4   // 4 rows on the matrix keypad
#define COLUMN_NUM         4   // 4 columns on the matrix keypad
#define STEPS_PER_REV    200   // 200 steps per revolution for the stepper motor
#define NUMBER_OF_UREVS 0.615  // 0.615 unlocking revolutions for the stepper motor
#define NUMBER_OF_LREVS 0.615  // 0.615 locking revolutions for the stepper motor

#define DIR 26            // D26 on ESP32
#define STEP 25           // D25 on ESP32

#define GREEN_LED    13   // D14 on ESP32
#define RED_LED      14   // D13 on ESP32

#define ROW_ONE      18   // D18 on ESP32
#define ROW_TWO      17   // TX2 on ESP32
#define ROW_THREE    16   // RX2 on ESP32
#define ROW_FOUR     4    // D4  on ESP32

#define COLUMN_ONE   23   // D23 on ESP32
#define COLUMN_TWO   22   // D22 on ESP32
#define COLUMN_THREE 21   // D21 on ESP32
#define COLUMN_FOUR  19   // D19 on ESP32


// Defining the key layout
// The key layout is a 4x4 matrix
char key_layout[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte pin_rows[ROW_NUM] = {ROW_ONE, ROW_TWO, ROW_THREE, ROW_FOUR};                  // The ESP32 pins connect to the row pins
byte pin_column[COLUMN_NUM] = {COLUMN_ONE, COLUMN_TWO, COLUMN_THREE, COLUMN_FOUR}; // The ESP32 pins connect to the column pins

Keypad keypad = Keypad(makeKeymap(key_layout), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String correct_password = "0102"; // my password
String input_password;                  // the input password

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  input_password.reserve(32); // maximum password size is 32, change if needed
  pinMode(GREEN_LED, OUTPUT); // set the green LED pin as output
  pinMode(RED_LED, OUTPUT);   // set the red LED pin as output
  pinMode(STEP, OUTPUT);      // set the stepper motor step pin as output
  pinMode(DIR, OUTPUT);       // set the stepper motor direction pin as output
}

// the loop function runs over and over again forever
void loop() {
  char key = keypad.getKey();

  if (key) {
    // print out the key that was pressed and turn on the red LED
    // to show the user that a key was pressed
    Serial.println("Key pressed: " + String(key));
    digitalWrite(RED_LED, HIGH);
    delay(25);
    digitalWrite(RED_LED, LOW);

    // check if the key is a number, the reset key, or the enter key
    if (key == '*') {
      Serial.println("Reset button pressed. Password cleared.");
      input_password = ""; // reset the input password
    } else if (key == '#') {
      if (input_password == correct_password) {
        // the correct password was entered, so flash the green LED
        Serial.println("The correct password! Flashing Green LED.");

        // turn on the green LED and spin the stepper motor in one direction
        digitalWrite(GREEN_LED, HIGH);
        Serial.println("Green LED is on.");
        digitalWrite(DIR, LOW);
        Serial.println("Spinning Anti-Clockwise... unlocking the door.");
        for(int i = 0; i < NUMBER_OF_UREVS * STEPS_PER_REV; i++)
        {
          digitalWrite(STEP, HIGH);
          delayMicroseconds(1500);
          digitalWrite(STEP, LOW);
          delayMicroseconds(1500);
        }

        // holding the door open, giving the user time to open the door
        delay(5000); 

        // turn off the green LED and spin the stepper motor in the opposite direction
        digitalWrite(GREEN_LED, LOW);
        Serial.println("Green LED is off.");
        digitalWrite(DIR, HIGH);
        Serial.println("Spinning Clockwise... locking the door.");
        for(int i = 0; i < NUMBER_OF_LREVS * STEPS_PER_REV; i++)
        {
          digitalWrite(STEP, HIGH);
          delayMicroseconds(2000);
          digitalWrite(STEP, LOW);
          delayMicroseconds(2000);
        }
      } else {
        // the incorrect password was entered, so flash the red LED
        Serial.println("The incorrect password! Try again. Flashing Red LED.");
        digitalWrite(RED_LED, HIGH);
        Serial.println("Red LED is on.");
        delay(1500);
        digitalWrite(RED_LED, LOW);
        Serial.println("Red LED is off.");
      }

      input_password = ""; // reset the input password
    } else {
      input_password += key; // append new character to input password string
    }
  }
}