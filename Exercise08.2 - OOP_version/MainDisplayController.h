/// FILE NAME:      MainDisplayController.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#ifndef MAINDISPLAYCONTROLLER_H
#define MAINDISPLAYCONTROLLER_H

// #define NUMBER_OF_MAINVIEW_PARAMETERS 3

#include "Arduino.h"
#include "ControllersBase.h"
#include "MainDisplay.h"
#include "MainDisplayView.h"
#include "States.h"

class MainDisplayController : public ControllersBase{
    public:
        MainDisplayController(MainDisplay* mainDisplay, MainDisplayView* mainDisplayView);
        States onLeftButtonPress();
        States onRightButtonPress();
        void onUpButtonPress();
        void onDownButtonPress();
        void MainIndex(int* selectedIndex = 0, String headerName = "Menu");
    private:
        MainDisplay* _mainDisplay;
        MainDisplayView* _mainDisplayView;
        int _selectedIndex = 0;
};

#endif //MAINDISPLAYCONTROLLER_H