/// FILE NAME:      FileIO.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        03.08.2018
/// MODIFIED:       03.08.2018

#include "FileIO.h"

String FileIO::getDataFileName(){
    return _dataFileName;
}

void FileIO::setDataFileName(String dataFileName){
    this -> _dataFileName = dataFileName;
}

int FileIO::getFreq(){
    return _freq;
}

void FileIO::setFreq(int freq){
    this -> _freq = freq;
}

bool FileIO::getLineMode(){
    return _linesEnabled;
}

void FileIO::setLineMode(bool linesEnabled){
    this -> _linesEnabled = linesEnabled;
}


bool FileIO::getScrollingEnabled(){
    return _scrollingEnabled;
}

void FileIO::setScrolling(bool scrollingEnabled){
    this -> _scrollingEnabled = scrollingEnabled;
}

int FileIO::getPointCount()
{
    return _pointCount;
}

void FileIO::setPointCount(int pointCount)
{
    this->_pointCount = pointCount;
}