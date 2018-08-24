/// FILE NAME:      RecorderController.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        03.08.2018
/// MODIFIED:       03.08.2018

#ifndef RECORDERCONTROLLER_H
#define RECORDERCONTROLLER_H

#include "RecorderView.h"
#include "FileIO.h"

class RecorderController{
    public:
        RecorderController(RecorderView* recorderView, FileIO* fileIO);

        void InitializeRecorder();
        void getNewGraphData();

        unsigned long getRecordPeriod();
        void setRecordPeriod(FileIO* fileIO);
        void setDefaultDelayTime(int delay);

    private:
        int _defaultDelayTime;
        unsigned long _recordPeriod;

        FileIO* _fileIO;
        RecorderView* _recorderView;
};

#endif // RECORDERCONTROLLER_H