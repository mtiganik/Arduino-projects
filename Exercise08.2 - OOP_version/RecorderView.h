/// FILE NAME:      RecorderView.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        03.08.2018
/// MODIFIED:       03.08.2018

#ifndef RECORDERVIEW_H
#define RECORDERVIEW_H

#include <arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class RecorderView
{
    public:
        RecorderView(Adafruit_SSD1306 *display);

    private:
        Adafruit_SSD1306 *_display;

};

#endif //RECORDERVIEW_H

