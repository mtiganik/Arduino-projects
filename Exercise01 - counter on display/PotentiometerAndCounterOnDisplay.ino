/*
    Project: 7seg LED numeric indicator
    Author: Mihkel Tiganik
    Date: 18.07.2018
    Hardware used:
      Arduino MEGA board
      4 digit 7seg display
      5k linear potentiometer
      Pushbutton
      1 kOhm resistors connected to pushbutton and LED display 
    Description: 
      Read potentiometer values to 4 digit 7seg display
    Add pushButton, which, when pressed, starts countdown
    When pushbutton is pressed again by short time, it starts / stops the countdown
    If pushbutton is pressed long, it will restart countdown
    When countdown reaches to 0, potentiometer value is displayed again. 
    Potentiometer has 2 scales
*/

int downCounter = 35;     // downCounter maximum value
int potentiometerMaxValue = 5000; // We use 5k potentiometer

int interruptPin = 18;  // pushButton pin
int interruptVector = 5; // For Arduino MEGA 2560 pin 18 interrupt is 5

// Connect potentiometer reader to A0 pin

const int debounceTime = 50;
const int lowPressTime = 800;

//display pins
int segA = 2;  // >>  11
int segB = 3; // >>  7
int segC = 4; // >>  4
int segD = 5;  // >>  2
int segE = 6;  // >>  1
int segF = 7;  // >>  10
int segG = 8; // >>  5
int segPt = 9; // >>  3
//------------//

//display digit
int d1 = 10;   // >> 12
int d2 = 11;   // >> 9
int d3 = 12;   // >> 8
int d4 = 13;  // >> 6
//------------//


int sensorTimeCounter = 0; // for smooth potentiometer values
int delayTime = 5000; //delayTime <Don't change it, if you don't know where is it!>
int sensorValue;
int i = 0;

bool potentioMeterRegime = true;
volatile int state = LOW;
bool shortPress = false;
bool isStartMode = true;
volatile int longPress = LOW;
volatile unsigned long current_high;
volatile unsigned long current_low;

//=============================================//
//init all pin used
void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  attachInterrupt(interruptVector, pin_CHANGE, CHANGE);

  sensorValue = analogRead(A0);  // Potentiometer reading pin
}


/////
////
//=============================================//
void loop() {

  if (shortPress || longPress) {
    down(downCounter / 1000, (downCounter % 1000) / 100 , (downCounter % 100) / 10, (downCounter % 10));
    //down(0, 0, 3, 0);
  
    shortPress = false;
    isStartMode = false;
    potentioMeterRegime = true;
  }

  if (potentioMeterRegime) {
    sensorTimeCounter++;
    if (sensorTimeCounter % 10 == 0) {
      sensorValue = analogRead(A0); // Potentiometer reading pin
    }
    float voltage = sensorValue * (potentiometerMaxValue / 1023.0);
    if (voltage > 1000)
    {
      Write10kLimit(voltage);
    } else
    {
      Write1kLimit(voltage);
    }
  }

}

void pin_CHANGE() {
  potentioMeterRegime = false;
  if (digitalRead(interruptPin) == HIGH) {
    current_high = millis();
    state = HIGH;
  }
  // if digitalread shows LOW, then button is released
  if (digitalRead(interruptPin) == LOW && state == HIGH)
  {
    current_low = millis();
    if ((current_low - current_high) > debounceTime && (current_low - current_high) < lowPressTime)
    {
      shortPress = true;
      isStartMode = !isStartMode;
      longPress = false;
      potentioMeterRegime = false;
      state = LOW;
    }
    else if ((current_low - current_high) >= lowPressTime && (current_low - current_high) < 4000)
    {
      longPress = true;
      shortPress = false;
      potentioMeterRegime = false;
      isStartMode = true;
      state = LOW;
    }
  }

}

//
//=============================================//
//Write a Number - writeN(1,9,9,0) -> 1990
void writeN(int a, int b, int c, int d) {
  selectDwriteL(1, a, false);
  selectDwriteL(2, b, false);
  selectDwriteL(3, c, false);
  selectDwriteL(4, d, false);
}

//=============================================//
//Make a Number Number Countdown (Timers).
void down(int a, int b, int c, int d) {
  while (a != -1) {
    while (b != -1) {
      while (c != -1) {
        while (d != -1) {
          while (i < 10) { // i here is like a timer ! because we can't use delay function
            if (isStartMode) {
              i++;
            }
            if (longPress) {
              longPress = false;
              down(downCounter / 1000, (downCounter % 1000) / 100 , (downCounter % 100) / 10, (downCounter % 10));
              return;
            }
            selectDwriteL(1, a, false);
            selectDwriteL(2, b, false);
            selectDwriteL(3, c, false);
            selectDwriteL(4, d, false);
          }
          i = 0;
          d--;
        }
        d = 9;
        c--;
      }
      c = 9;
      b--;
    }
    b = 9;
    a--;
  }
}

void Write1kLimit(float fVoltage)
{
  int iVoltage = 10 * fVoltage;
  if (iVoltage > 1000)
  {
    selectDwriteL(1, iVoltage / 1000, false);
    iVoltage = iVoltage % 1000;
  }
  selectDwriteL(2, iVoltage / 100, false);
  iVoltage = iVoltage % 100;
  selectDwriteL(3, iVoltage / 10, true);
  iVoltage = iVoltage % 10;
  selectDwriteL(4, iVoltage, false);
}

void Write10kLimit(float fVoltage)
{
  int iVoltage = fVoltage;
  selectDwriteL(1, iVoltage / 1000, false);
  iVoltage = iVoltage % 1000;
  selectDwriteL(2, iVoltage / 100, false);
  iVoltage = iVoltage % 100;
  selectDwriteL(3, iVoltage / 10, false);
  iVoltage = iVoltage % 10;
  selectDwriteL(4, iVoltage, true);

}

//=============================================//
//Select Wich Digit (selectD) is going to Display (writeL)
void selectDwriteL(int d, int l, bool dp) {
  switch (d) { // choose a digit
    case 0: digitalWrite(d1, LOW); //case 0 - All ON
      digitalWrite(d2, LOW);
      digitalWrite(d3, LOW);
      digitalWrite(d4, LOW);
      break;
    case 1: digitalWrite(d1, LOW);//case 1 - Digit Number 1
      digitalWrite(d2, HIGH);
      digitalWrite(d3, HIGH);
      digitalWrite(d4, HIGH);
      break;
    case 2: digitalWrite(d1, HIGH);//case 1 - Digit Number 2
      digitalWrite(d2, LOW);
      digitalWrite(d3, HIGH);
      digitalWrite(d4, HIGH);
      break;
    case 3: digitalWrite(d1, HIGH);//case 1 - Digit Number 3
      digitalWrite(d2, HIGH);
      digitalWrite(d3, LOW);
      digitalWrite(d4, HIGH);
      break;
    case 4: digitalWrite(d1, HIGH);//case 1 - Digit Number 4
      digitalWrite(d2, HIGH);
      digitalWrite(d3, HIGH);
      digitalWrite(d4, LOW);
      break;
  }

  switch (l) { // choose a Number
    case 0: zero(dp);
      break;
    case 1: one(dp);
      break;
    case 2: two(dp);
      break;
    case 3: three(dp);
      break;
    case 4: four(dp);
      break;
    case 5: five(dp);
      break;
    case 6: six(dp);
      break;
    case 7: seven(dp);
      break;
    case 8: eight(dp);
      break;
    case 9: nine(dp);
      break;
  }

  delayMicroseconds(delayTime); // delayTime for nice display of the Number !

}

//=============================================//
//shown one Number in the 4 Digit
void all(int n) {
  selectDwriteL(0, n, false);
}

//=============================================//
void zero(bool dp) {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, LOW);
  digitalWrite(segPt, dp ? HIGH : LOW);
}
//=============================================//
void one(bool dp) {
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
  digitalWrite(segPt, dp ? HIGH : LOW);
}
//=============================================//
void two(bool dp) {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, LOW);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, LOW);
  digitalWrite(segG, HIGH);
  digitalWrite(segPt, dp ? HIGH : LOW);
}
//=============================================//
void three(bool dp) {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, HIGH);
  digitalWrite(segPt, dp ? HIGH : LOW);
}
//=============================================//
void four(bool dp) {
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  digitalWrite(segPt, dp ? HIGH : LOW);
}
//=============================================//
void five(bool dp) {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  digitalWrite(segPt, dp ? HIGH : LOW);
}
//=============================================//
void six(bool dp) {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  digitalWrite(segPt, dp ? HIGH : LOW);
}
//=============================================//
void seven(bool dp) {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
  digitalWrite(segPt, dp ? HIGH : LOW);
}
//=============================================//
void eight(bool dp) {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  digitalWrite(segPt, dp ? HIGH : LOW);
}
//=============================================//
void nine(bool dp) {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  digitalWrite(segPt, dp ? HIGH : LOW);
}

