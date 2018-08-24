/// FILE NAME:      Joystick.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#include "Joystick.h"

// States Joystick::getCurrentState()
// {
//     return _currentState;
// }

bool Joystick::getLeftReleased()
{
    return _leftReleased;
}
bool Joystick::getUpReleased()
{
    return _upReleased;
}
bool Joystick::getRightReleased()
{
    return _rightReleased;
}
bool Joystick::getDownReleased()
{
    return _downReleased;
}
bool Joystick::getPushButtonReleased()
{
    return _pushButtonReleased;
}

// void Joystick::setCurrentState(States currentState){
//     this -> _currentState = currentState;
// }

void Joystick::setLeftReleased(bool leftReleased)
{
    this -> _leftReleased = leftReleased;
}
void Joystick::setUpReleased(bool upReleased)
{
    this -> _upReleased = upReleased;
}
void Joystick::setRightReleased(bool rightReleased)
{
    this -> _rightReleased = rightReleased;
}
void Joystick::setDownReleased(bool downReleased)
{
    this -> _downReleased = downReleased;
}
void Joystick::setPushButtonReleased(bool pushButtonReleased)
{
    this -> _pushButtonReleased = pushButtonReleased;
}