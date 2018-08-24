/// FILE NAME:      FileIO.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        03.08.2018
/// MODIFIED:       03.08.2018

#ifndef FILEIO_H
#define FILEIO_H
#define CONFIG_FILE "config.txt"

#include "Arduino.h"
#include "SdFat.h"

class FileIO{
    public:

    String getDataFileName();
    void setDataFileName(String dataFileName);

    int getFreq();
    void setFreq(int freq);

    bool getLineMode();
    void setLineMode(bool linesEnabled);

    bool getScrollingEnabled();
    void setScrolling(bool scrollingEnabled);

    int getPointCount();
    void setPointCount(int pointCount);

    private:
    String _dataFileName;
    int _pointCount;
    int _freq;
    bool _linesEnabled;
    bool _scrollingEnabled;
};

#endif //FILEIO_H