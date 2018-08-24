/*
    Project: Distance meter
    Author: Mihkel Tiganik
    Date: 20.07.2018
    Hardware:
      Arduino MEGA board
      LCD1602 display
      SSD1306 OLED display
      HC-SR04 Ultrasonic sensor
      Servo motor
    Description: 
      *Connect LCD1602 display with I2C Adapter
      *Connect SSD1306 OLED with I2C Adapter
      *Make HC-SR04 Ultrasonic sensor to read distance
      *Place Ultrasonic sensor on top of servo motor
      *Make servo motor to rotate 180 degrees
      *Display US sensor values to LCD and OLED display
      *For OLED display, show sensor values as cyclic radar values
*/


#include <LiquidCrystal_I2C.h> // for LCD 
#include <Servo.h>             // for servo motor
#include <SPI.h>              
#include <Wire.h>
#include <Adafruit_GFX.h>      // for OLED
#include <Adafruit_SSD1306.h>  // for OLED


#define MAX_DISTANCE 100       // Distance to show on OLED display
#define HALF_WIDTH 64          // OLED display half width. For calculating

// OLED display
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// LCD display
LiquidCrystal_I2C lcd(0x3F, 16, 2); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)

Servo servo;

// user defined 'angle' sign to display on LCD display
uint8_t angle_sign[8] = {0x6, 0x9 ,0x9, 0x6, 0x0, 0x0, 0x0};

byte servo_direction = HIGH;
int angle = 0;
const int trigPin = 9;    // Ultrasound trigger pin
const int echoPin = 10;   // Ultrasound echo pin
long duration;            // duration of ultrasound
int distanceCm;           // variable to convert US duration to distance
double displayRadius;     // = distanceCm * HALF_WIDTH / MAX_DISTANCE;

void setup() {
 display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 display.display();
 display.clearDisplay();

  resetOLEDDisplay();
  display.display();
  lcd.begin();
  lcd.createChar(0, angle_sign);
  servo.attach(8);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {

  // move servo
  if(servo_direction){
    angle++;
  }else{
    angle--;
  }
  checkServoDirection();
  servo.write(angle); 

  // get distance value
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  if(duration < 30000)
  {
    // convert echo duration time to distance
    distanceCm = duration * 0.034 / 2; 
  }
  // // Optional. Write US sensor value to Serial
  // Serial.println(distanceCm);

  // Write value to LCD screen
  lcd.setCursor(0, 0);      // Sets the location at which subsequent text written to the LCD will be displayed
  lcd.print("Distance: ");  // Prints string "Distance" on the LCD
  //lcd.setCursor(10,0);
  lcd.print(distanceCm);    // Prints the distance value from the sensor
  lcd.setCursor(13,0);
  lcd.print("cm");
  lcd.setCursor(0,1);
  lcd.write(224);
  lcd.print("=");
  lcd.print(angle);
  lcd.write(0);
  
  delay(10);

  // Write value to OLED display
  if(distanceCm <= MAX_DISTANCE){
    displayRadius = distanceCm * HALF_WIDTH / MAX_DISTANCE;
    display.drawPixel(HALF_WIDTH + displayRadius * cos(angle * PI / 180),
    HALF_WIDTH - displayRadius *sin(angle * PI / 180) ,WHITE);
    display.display();
  }
  delay(10);
}

void checkServoDirection(){
  if(angle >=179){
      // let servo go back
      servo_direction = LOW;
      resetOLEDDisplay();
  }else if(angle <= 1){
    servo_direction = HIGH;
    resetOLEDDisplay();
  }
}

void resetOLEDDisplay(){
  display.clearDisplay();
  display.drawLine(62,63,66,63,WHITE);
  display.drawLine(64, 63, 64, 61, WHITE);
  display.drawCircle(64,64,64,WHITE);
  display.display();
}


