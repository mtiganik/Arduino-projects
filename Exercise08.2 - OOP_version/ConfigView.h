/// FILE NAME:      ConfigView.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#ifndef CONFIGVIEW_H
#define CONFIGVIEW_H

#include <arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include "ViewBase.h"
#include "Config.h"



class ConfigView
{
  public:
    ConfigView(Adafruit_SSD1306 *display);
    void Index(Config* config, int selectedIndex, String headerName);

  private:
    Adafruit_SSD1306 *_display;

    // String _configMenuItems[5]{
    //     "Frequency",
    //     "Point count",
    //     "Line type",
    //     "Scrolling",
    //     "File name"};
};

#endif // CONFIGVIEW_H