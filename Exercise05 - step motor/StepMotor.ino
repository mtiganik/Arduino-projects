/*
    Project: Step motor
    Author: Mihkel Tiganik
    Date: 23.07.2018
    Hardware:
      Arduino MEGA board
      Step motor
      2 pushbuttons
    Description: Connect Step motor and 2 buttons
    When first button is pressed, move step motor by some constant speed
    When second button is pressed, change rotation of step motor
    Add possibility to send step count, speed and direction with serial port
*/

#include <AccelStepper.h>

// Stepper motor:
// input 1: pin 8
// input 2: pin 9
// input 3: pin 10
// input 4: pin 11
AccelStepper stepper(8, 8, 10, 9, 11); // FULL4Wire

#define DEBOUNCE 5  // button debouncer, how many ms to debounce, 5+ ms is usually plenty
#define button1 0
#define button2 1

#define serialDefaultInput 0
#define serialStepCount 1
#define serialSpeed 2
#define directionChanged 3
int serialInputMode = serialDefaultInput;

long stepCount =  64 * 64;

byte button1Pin = 2;
byte button2Pin = 3;
byte buttons[] = {button1Pin, button2Pin };

// This handy macro lets us determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)

// we will track if a button is just pressed, just released, or 'currently pressed'
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];

byte button1Pressed = LOW;
byte button2Pressed = LOW;

long newPosition = 0;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// default speed is gotten from analog input.
// if someone has declared new speed from serial, then this goes true.
byte speedFromSerial = false;
int speedFromSerialValue;

void setup() {
  byte i;

  // set up serial port
  Serial.begin(9600);

  inputString.reserve(200);

  // Make input & enable pull-up resistors on switch pins
  for (i = 0; i < NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  }

  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(1000);
  //stepper.setSpeed(200);

  Serial.println("'1' - step count, '2' - speed, '3' - direction");
}


void loop() {
  check_switches();      // when we check the switches we'll get the current state

  if (justreleased[button2] || serialInputMode == directionChanged) {
    if (serialInputMode == directionChanged) {
      serialInputMode = serialDefaultInput;
    }
    //Serial.println(" Just released");
    delay(5);
    button2Pressed = !button2Pressed;
  }

  if (justreleased[button1]) {
    delay(5);
    // Serial.println(" Just released");
    //button1Pressed = !button1Pressed;
    stepper.setMaxSpeed(speedFromSerial ? speedFromSerialValue : analogRead(A0));
    newPosition = stepper.currentPosition() + (button2Pressed ? stepCount : -stepCount ) ;
  }

  stepper.moveTo(newPosition);
  stepper.run();

  // loop to check user input in serial port
  if (stringComplete) {
    Serial.print("Entered: ");
    Serial.println(inputString);
    //int inputAsInt = inputString.toInt();
    if (serialInputMode == serialDefaultInput) {
      switch (inputString.toInt()) {
        case (1):
          Serial.println("Enter step count, 2048 is half rotation");
          serialInputMode = serialStepCount;
          break;
        case (2):+
          Serial.println("Enter speed, 500 to 1200 is good range");
          serialInputMode = serialSpeed;
          break;
        case (3):
          serialInputMode = directionChanged;
          Serial.println("Direction changed");
          break;
        default:
          Serial.println("Invalid input. Please enter: ");
          Serial.println("'1' - step count, '2' - speed, '3' - direction");
          break;
      }
    } else if (serialInputMode == serialStepCount) {
      int newValue;

      newValue = inputString.toInt();
      if (newValue < 0 || newValue > 10000) {
        Serial.println("Error, Value cannot be lower than 0 or Higher than 10000");
        Serial.println("Enter step count");
      } else {
        stepCount = newValue;
        serialInputMode = serialDefaultInput;
      }
    } else if (serialInputMode == serialSpeed) {
      int newSpeedValue;
      newSpeedValue = inputString.toInt();

      if (newSpeedValue < 0 || newSpeedValue > 1200) {
        Serial.println("Error, value cannot be lower than 0 or higher than 1200");
        Serial.println("Enter speed");
      } else {
        speedFromSerial = true;
        speedFromSerialValue = newSpeedValue;
        serialInputMode = serialDefaultInput;
      }
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      inputString += (char)inChar;
    }
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void check_switches()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;
  if (millis() < lasttime) {
    lasttime = millis(); // we wrapped around, lets just try again
  }

  if ((lasttime + DEBOUNCE) > millis()) {
    return; // not enough time has passed to debounce
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();

  for (index = 0; index < NUMBUTTONS; index++) {
    justpressed[index] = 0;       // when we start, we clear out the "just" indicators
    justreleased[index] = 0;

    currentstate[index] = digitalRead(buttons[index]);   // read the button
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        // just released
        justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }
    //Serial.println(pressed[index], DEC);
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}
