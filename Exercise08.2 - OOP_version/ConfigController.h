/// FILE NAME:      ConfigController.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#ifndef CONFIGCONTROLLER_H
#define CONFIGCONTROLLER_H

//#define NUMBER_OF_CONFIG_PARAMETERS 5

#include "Arduino.h"
//#include "SdFat.h"

#include "Config.h"
#include "ConfigView.h"
#include "ControllersBase.h"
#include "States.h"

class ConfigController: public ControllersBase{
    public:
        ConfigController(Config* config, ConfigView* configView);
        void ConfigIndex(int* selectedIndex = 0, String headerName = "Config");
        States onLeftButtonPress();
        States onRightButtonPress();
        void onUpButtonPress();
        void onDownButtonPress();

    private:
        int _selectedIndex = 0;
        Config* _config;
        ConfigView* _configView;

};


#endif // CONFIGCONTROLLER_H