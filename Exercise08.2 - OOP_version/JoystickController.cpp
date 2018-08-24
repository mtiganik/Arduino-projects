/// FILE NAME:      JoystickController.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#include "JoystickController.h"

JoystickController::JoystickController(Joystick* joystick, 
            ConfigController* configController, 
            MainDisplayController* mainDisplayController,
            RecorderController* recorderController )
{
    this -> _joystick = joystick;
    this -> _configController = configController;
    this -> _mainDisplayController = mainDisplayController;
    this -> _recorderController = recorderController;
}


void JoystickController::updateMenu(){
    if(_joystick -> getUpReleased()){
        switch(_currentState){
        case (States::config):
            _configController->onUpButtonPress();
            break;
        case (States::mainDisplay):
            _mainDisplayController->onUpButtonPress();
            break;
        default:
            Serial.println("Currenstate enum working");
            break;
        }
    } 
    if(_joystick -> getDownReleased()){
        switch(_currentState)
        {
        case (States::config):
            _configController->onDownButtonPress();
            break;
        case (States::mainDisplay):
            _mainDisplayController->onDownButtonPress();
            break;
        default:
            Serial.println("default");
            break;
        }
    }
}

void JoystickController::getNewActionMethod()
{
     if(_joystick -> getLeftReleased()){
        switch(_currentState){
        case (States::config):
            this->_currentState = _configController -> onLeftButtonPress();
            break;
        case (States::mainDisplay):
            this->_currentState = _mainDisplayController -> onLeftButtonPress();
            break;
        default:
            Serial.println("Currenstate enum working");
            break;
        }
    } 
    if(_joystick -> getRightReleased()){
        switch(_currentState)
        {
        case (States::config):
            this->_currentState = _configController -> onRightButtonPress();
            break;
        case (States::mainDisplay):
            this->_currentState = _mainDisplayController -> onRightButtonPress();
            break;
        default:
            Serial.println("default");
            break;
        }
    }
}

void JoystickController::updateNewActionMethod()
{
    switch(_currentState){
        case(States::config):
            _configController -> ConfigIndex();
            break;
        case(States::mainDisplay):
            _mainDisplayController -> MainIndex();
            break;
        case(States::newGraph):
            _recorderController -> InitializeRecorder();
            _recorderController -> getNewGraphData();
        default:
            break;
    }
}

void JoystickController::setCurrentState(States state){
    this -> _currentState = state;
}

States JoystickController::getCurrentState(){
    return _currentState;
}

void JoystickController::setPins(uint8_t xPin, uint8_t yPin, uint8_t swPin)
{
    this -> _xPin = xPin;
    this -> _yPin = yPin;
    this -> _swPin = swPin;
    pinMode(swPin, INPUT);
    digitalWrite(swPin, HIGH);
}




bool JoystickController::hasUpDownButtonReleased()
{
    if( _joystick->getUpReleased() ||  _joystick->getDownReleased() )
    {
        return true;
    }
    else
    {
        return false;
    }
}

 bool JoystickController::hasLeftRightButtonReleased()
 {
      if( _joystick->getLeftReleased() ||  _joystick->getRightReleased() )
    {
        return true;
    }
    else
    {
        return false;
    }
 }

void JoystickController::checkSwitches()
{
    static byte previousstate[NUMBUTTONS];
    static byte currentstate[NUMBUTTONS];

    static long lastTime;

    if (millis() < lastTime)
    {
        lastTime = millis(); // initialize lastTime
    }
    if ((lastTime + DEBOUNCE_TIME) > millis())
    {
        return; // not enought time has passed for debouncing
    }
    delay(10);
    // we waited DEBOUNCE_TIME, lets reset timer
    lastTime = millis();

    currentstate[LEFT] = analogRead(_xPin) < 100 ? 0 : 1;
    currentstate[UP] = analogRead(_yPin) < 100 ? 0 : 1;
    currentstate[RIGHT] = analogRead(_xPin) > 800 ? 0 : 1;
    currentstate[DOWN] = analogRead(_yPin) > 800 ? 0 : 1;
    currentstate[PUSH] = digitalRead(_swPin);

    for (int index = 0; index < NUMBUTTONS; index++)
    {
        justreleased[index] = 0;

        if (currentstate[index] == previousstate[index])
        {
         
            if ((pressed[index] == HIGH) && (currentstate[index] == HIGH))
            {
                // just released
                justreleased[index] = 1;
            }
            pressed[index] = !currentstate[index]; // remember, digital HIGH means NOT pressed
        }
        previousstate[index] = currentstate[index]; // keep a running tally of the buttons
    }

    // parse values to model
    _joystick -> setLeftReleased(justreleased[LEFT]);
    _joystick -> setUpReleased(justreleased[UP]);
    _joystick -> setRightReleased(justreleased[RIGHT]);
    _joystick -> setDownReleased(justreleased[DOWN]);
    _joystick -> setPushButtonReleased(justreleased[PUSH]);
    
}