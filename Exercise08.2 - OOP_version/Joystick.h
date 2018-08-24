/// FILE NAME:      Joystick.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#ifndef JOYSTICK_H
#define JOYSTICK_H



class Joystick{
    public:
       // States getCurrentState();
        bool getLeftReleased();
        bool getUpReleased();
        bool getRightReleased();
        bool getDownReleased();
        bool getPushButtonReleased();

       // void setCurrentState(States currentState);
        void setLeftReleased(bool leftReleased);
        void setUpReleased(bool upReleased);
        void setRightReleased(bool rightReleased);
        void setDownReleased(bool downReleased);
        void setPushButtonReleased(bool pushButtonReleased);

    private:
   // States _currentState;
    bool _leftReleased;
    bool _upReleased;
    bool _rightReleased;
    bool _downReleased;
    bool _pushButtonReleased;
};


#endif //JOYSTICK_H