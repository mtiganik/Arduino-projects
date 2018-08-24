/// FILE NAME:      RecorderView.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        03.08.2018
/// MODIFIED:       03.08.2018

#include "RecorderView.h"

RecorderView::RecorderView(Adafruit_SSD1306 *display)
{
    this -> _display = display;
}