/// FILE NAME:      RecorderController.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        03.08.2018
/// MODIFIED:       03.08.2018

#include "RecorderController.h"

RecorderController::RecorderController(RecorderView* recorderView, FileIO* fileIO)
{
    this -> _recorderView = recorderView;
    this -> _fileIO = fileIO;
}

void RecorderController::InitializeRecorder()
{

}

void RecorderController::getNewGraphData()
{

}

unsigned long RecorderController::getRecordPeriod()
{
    return _recordPeriod;
}

void RecorderController::setRecordPeriod(FileIO* fileIO)
{
    if(1000 / (_fileIO -> getFreq()) > _defaultDelayTime){
        _recordPeriod = 1000 / _fileIO -> getFreq() - _defaultDelayTime;
    }else{
        _recordPeriod = 0;
    }
}

void  RecorderController::setDefaultDelayTime(int delay)
{
    _defaultDelayTime = delay;
}