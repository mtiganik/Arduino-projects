/// FILE NAME:      JoystickController.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#ifndef JOYSTICKCONTROLLER_H
#define JOYSTICKCONTROLLER_H

#include "Arduino.h"
#include "Joystick.h"
#include "ConfigController.h"
#include "MainDisplayController.h"
#include "RecorderController.h"

#define DEBOUNCE_TIME 10
#define LEFT                    0
#define UP                      1
#define RIGHT                   2
#define DOWN                    3
#define PUSH                    4
#define NUMBUTTONS              5



class JoystickController
{
  public:
    JoystickController(Joystick *joystick, 
    ConfigController* configController, 
    MainDisplayController* mainDisplayController,
    RecorderController* recorderController );
    void updateMenu();
    void setPins(uint8_t xPin, uint8_t yPin, uint8_t swPin);
    bool hasUpDownButtonReleased();
    bool hasLeftRightButtonReleased();
    void getNewActionMethod();
    void updateNewActionMethod();
    void setCurrentState(States state);
    void checkSwitches();

    States getCurrentState();

  private:
    
    States _currentState;

    byte pressed[NUMBUTTONS], justreleased[NUMBUTTONS];
    
    Joystick *_joystick;
    uint8_t _xPin;
    uint8_t _yPin;
    uint8_t _swPin;

    MainDisplayController *_mainDisplayController;
    ConfigController *_configController;
    RecorderController* _recorderController;

    
};

#endif //JOYSTICKCONTROLLER_H