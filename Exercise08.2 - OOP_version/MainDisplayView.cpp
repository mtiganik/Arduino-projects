/// FILE NAME:      MainDisplayView.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018


#include "MainDisplayView.h"

MainDisplayView::MainDisplayView(Adafruit_SSD1306 *display)
{
    this -> _display = display;
}

void MainDisplayView::Index(MainDisplay* mainDisplay, int selectedIndex, String headerName)
{
     _display -> clearDisplay();
    _display -> setCursor(10, 1);
    _display -> setTextColor(WHITE);
    _display -> println(headerName);
    _display -> drawLine(0, 8, 123, 8, WHITE);
    if (mainDisplay -> itemsCount < 5)
    {
        _display -> println();
    }
    for (int i = (selectedIndex < 6 ? 0 : selectedIndex - 5); i < mainDisplay -> itemsCount; i++)
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
        _display -> println(mainDisplay -> MainMenuItems[i]);
    }
    _display -> display();
}
