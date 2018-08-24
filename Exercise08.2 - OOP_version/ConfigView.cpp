/// FILE NAME:      ConfigView.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#include "ConfigView.h"

ConfigView::ConfigView(Adafruit_SSD1306* display)
{
    this -> _display = display;
}
void ConfigView::Index(Config* config, int selectedIndex, String headerName)
{
    _display -> clearDisplay();
    _display -> setCursor(10, 1);
    _display -> setTextColor(WHITE);
    _display -> println(headerName);
    _display -> drawLine(0, 8, 123, 8, WHITE);
    if (config->itemsCount < 5)
    {
        _display -> println();
    }
    for (int i = (selectedIndex < 6 ? 0 : selectedIndex - 5); i < config->itemsCount; i++)
    {
        if (i == selectedIndex)
        {
            _display -> setTextColor(BLACK, WHITE);
            _display -> print(">|");
        }
        else
        {
            _display -> setTextColor(WHITE);
            _display -> print(" |");
        }
        _display -> println(config->ConfigMenuItems[i]);
    }
    _display -> display();
}
