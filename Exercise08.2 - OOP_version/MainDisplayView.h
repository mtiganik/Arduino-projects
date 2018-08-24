/// FILE NAME:      MainDisplayView.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#ifndef MAINDISPLAYVIEW_H
#define MAINDISPLAYVIEW_H

//#include "MainDisplayController.h"
#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ViewBase.h"
#include "MainDisplay.h"

class MainDisplayView
{
  public:
    MainDisplayView(Adafruit_SSD1306 *display);
    void Index(MainDisplay* mainDisplay, int selectedIndex, String headerName);

  private:
    Adafruit_SSD1306 *_display;

    // String _mainMenuItems[5]{
    //     "Start new",
    //     "Show graph",
    //     "Config"
    //     };
};

#endif //MAINDISPLAYVIEW_H