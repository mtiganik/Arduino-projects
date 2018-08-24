/*
    Project: Digital Dimmer
    Author: Mihkel Tiganik
    Date: 20.07.2018
    Hardware:
      Arduino MEGA board
      LED connected with 1k resistor
      pushbutton to simulate ligth switch
      pushbutton to simulate moving sensor
    Description: Reverse engineering to implement ligth ballast.
    Trying to simulate how ligth in meeting rooms is working:
      *if ligth switch is pressed for short time, LED goes on / off
      *if ligth switch is presed for long time, LED goes brigther / dimmer 
        until switch is released
      *if moving sensor pushbutton is pressed, then ligth goes on for 10 seconds. 
        For that time ligth switch state is ignored 
*/

#define MotionPin 2
#define MotionPinVector 0
#define SwitchPin 3
#define SwitchPinVector 1

int ledPin = 9;    // LED connected to digital pin 9

int start_counter = 0;
int debounce_counter = 0;
int long_press_counter = 0;
int long_press_end = 0;
byte button_pressed = LOW;
byte long_press = LOW;
byte long_press_direction = HIGH;
byte button_released = LOW;
byte led_on = LOW;
byte motion_detected = LOW;

int motion_LED_brigthness = 120;
int motion_time_counter = 0;
int motion_start_counter = 0;
int lowPressTime = 800;
int debounce = 20;
int fadeValue;
int photoResistorValue;
int previousValue;
void setup() {

  attachInterrupt(SwitchPinVector, on_button_press, CHANGE);
  attachInterrupt(MotionPinVector, on_motion, FALLING);

  Serial.begin(9600);
  previousValue = fadeValue = 125;
}

void loop() {

  if (button_pressed && !button_released) {
    long_press_counter = millis();
    Serial.println("button pressed");


    if (long_press_counter - long_press_end > 2000) {
      Serial.println("LONG press");
      long_press = HIGH;
      fadeValue = previousValue;
      led_on = HIGH;
      if (long_press_direction == HIGH) {
        while (button_pressed && fadeValue < 255 && button_released == false) {
          analogWrite(ledPin, fadeValue);
          delay(50);
          fadeValue = fadeValue + 5;
          Serial.println("LED getting brigther");
        }
      }
      else {
        while (button_pressed && fadeValue > 20 && button_released == false) {
          analogWrite(ledPin, fadeValue);
          delay(50);
          Serial.println("LED getting dimmer");
          fadeValue = fadeValue - 5;
        }
      }
      long_press_direction = !long_press_direction;
      // if we reached this far, then no need to do anything with long press
      button_pressed = false;
      Serial.println("Reached end of loop");

      // Need to save counter values aswell
      long_press_end = long_press_counter;
      long_press_counter = millis();
    }
  } else {
    long_press_end = long_press_counter;
    long_press_counter = millis();
  }
  if (button_released) {
    Serial.println("button released");

    previousValue = fadeValue;
    button_released = false;
    if (long_press) {
      long_press = LOW;
    } else {
      if (led_on) {
        led_on = LOW;
        Serial.println("LED OFF");
        analogWrite(ledPin, 0);
      } else {
        led_on = HIGH;
        Serial.println("LED ON");
        analogWrite(ledPin, previousValue);
      }
    }
  }

  if (motion_detected) {
    if (led_on) {
      Serial.println("Motion detected, but LED is on. Don't do anything");
      motion_detected = LOW;
    } else {
      analogWrite(ledPin, motion_LED_brigthness);
      motion_time_counter = millis();
      if ((motion_time_counter - motion_start_counter) % 1000 == 0) {
        Serial.print("motion LED on "); Serial.println((motion_time_counter - motion_start_counter) / 1000);
      }
      if ((motion_time_counter - motion_start_counter) > 10000) {
        Serial.println("motion LED time is up");
        motion_detected = LOW;
        analogWrite(ledPin, 0);
      }
    }
  }
}

void on_motion() {
  debounce_counter = millis();
  if (debounce_counter - start_counter > debounce) {
    motion_detected = HIGH;
    motion_start_counter = millis();
  }

  start_counter = debounce_counter;
}


void on_button_press() {

  debounce_counter = millis();
  if (debounce_counter - start_counter > debounce)
  {
    if (digitalRead(SwitchPin) == LOW) {

      button_pressed = true;
    }
  }
  if (debounce_counter - start_counter > debounce)
  {
    if (digitalRead(SwitchPin) == HIGH) {

      button_pressed = false;
      button_released = true;
    }
  }
  start_counter = debounce_counter;
}



