/// FILE NAME:      MainDisplayController.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#include "MainDisplayController.h"

MainDisplayController::MainDisplayController(MainDisplay* mainDisplay, MainDisplayView* mainDisplayView)
{
    this -> _mainDisplay = mainDisplay;
    this -> _mainDisplayView = mainDisplayView;
}


States MainDisplayController::onLeftButtonPress()
{
    return States::mainDisplay;
}
States MainDisplayController::onRightButtonPress()
{
    if(_selectedIndex == 2){
        return States::config;
    }else if(_selectedIndex == 0){
        return States::newGraph;
    }else
    {
        return States::mainDisplay;
    }
    
}
void MainDisplayController::onUpButtonPress()
{
     _selectedIndex--;
    MainIndex(&_selectedIndex, "Main view");
}
void MainDisplayController::onDownButtonPress()
{
     _selectedIndex++;
    MainIndex(&_selectedIndex, "Main view");
}

void MainDisplayController::MainIndex(int* selectedIndex, String headerName)
{
    if(*selectedIndex == -1){
        *selectedIndex = _mainDisplay -> itemsCount -1;
    }else if(*selectedIndex == _mainDisplay -> itemsCount){
        *selectedIndex = 0;
    }
    _mainDisplayView -> Index(_mainDisplay, *selectedIndex, headerName);
}