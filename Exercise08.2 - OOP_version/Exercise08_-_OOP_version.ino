/// FILE NAME:      Exercise08_-_OOP_Version.ino
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

// This program functionality is same as it is for other Exercise08,
// except this time MVC OOP was used. 

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "Config.h"
#include "ConfigController.h"

#include "FileIO.h"
#include "FileIOController.h"

#include "MainDisplay.h"
#include "MainDisplayView.h"
#include "MainDisplayController.h"

#include "RecorderView.h"
#include "RecorderController.h"

#include "JoyStick.h"
#include "JoystickController.h"
#define CS_PIN SS
#define JOYSTICK_X_PIN         A0
#define JOYSTICK_Y_PIN         A1
#define JOYSTICK_SW_PIN         8
#define DEFAULT_DELAY_TIME     10

Adafruit_SSD1306 display(4);

FileIO FIO;
FileIOController FIOC(&FIO);

Config config;
ConfigView CV(&display);
ConfigController CC(&config, &CV);

MainDisplay MD;
MainDisplayView MDV(&display);
MainDisplayController MDC(&MD, &MDV);

RecorderView RV(&display);
RecorderController RC(&RV, &FIO);

Joystick joystick;
JoystickController JC(&joystick, &CC, &MDC, &RC);

unsigned long timeNow;

void setup(){
  StartDisplay();
  
  JC.setPins(JOYSTICK_X_PIN, JOYSTICK_Y_PIN, JOYSTICK_SW_PIN);
  RC.setDefaultDelayTime((int)DEFAULT_DELAY_TIME);  // for frequency synchronization

  Serial.begin(9600);

  if(!FIOC.SDExists(CS_PIN)){
    Serial.println("Error finding SD card");
    return;
  }
  FIOC.getDataFromConfigFile(FIO);
  
 // CC.ConfigIndex(&firstIndex,"Hello");
  JC.setCurrentState(States::mainDisplay);
  //Serial.println(config.getDataFileName());
  int firstIndex = 0;
  MDC.MainIndex(&firstIndex, "Hello");
}
bool hasAnyReleased;
void loop()
{
  JC.checkSwitches();
  if(JC.hasUpDownButtonReleased()){
    JC.updateMenu();
  }
  if(JC.hasLeftRightButtonReleased()){
    JC.getNewActionMethod();
    JC.updateNewActionMethod();
  }

  // different loop for recording
  if(JC.getCurrentState() == States::newGraph){
    if (millis() > timeNow + RC.getRecordPeriod()) {
      timeNow = millis();
      RC.getNewGraphData();
    }
  }

  delay(DEFAULT_DELAY_TIME);
}

void StartDisplay()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.setTextSize(1);
  display.clearDisplay();
}