/// FILE NAME:      Config.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"

class Config
{
    public:
     String ConfigMenuItems[5]{
        "Frequency",
        "Point count",
        "Line type",
        "Scrolling",
        "File name"};
    int itemsCount = 5;
  

    private:
  

};

#endif // CONFIG_H