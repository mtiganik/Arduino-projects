/// FILE NAME:      ConfigController.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#include "ConfigController.h"

ConfigController::ConfigController(Config* config, ConfigView* configView){
    this->_config = config;
    this->_configView = configView;
}

States ConfigController::onLeftButtonPress()
{
    _selectedIndex = 0;
    return States::mainDisplay;
}
States ConfigController::onRightButtonPress()
{
    return States::config;
}
void ConfigController::onUpButtonPress()
{
    _selectedIndex--;
    ConfigIndex(&_selectedIndex, "Config");

}
void ConfigController::onDownButtonPress()
{
    _selectedIndex++;
    ConfigIndex(&_selectedIndex, "Config");
}

void ConfigController::ConfigIndex(int* selectedIndex, String headerName){
    if(*selectedIndex == -1){
        *selectedIndex = _config->itemsCount -1;
    }else if(*selectedIndex == _config->itemsCount){
        *selectedIndex = 0;
    }
    _configView -> Index(_config, *selectedIndex, headerName);
}
