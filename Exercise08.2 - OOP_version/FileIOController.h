/// FILE NAME:      FileIOController.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        03.08.2018
/// MODIFIED:       03.08.2018

#ifndef FILEIOCONTROLLER_H
#define FILEIOCONTROLLER_H

#include "Arduino.h"
#include "SdFat.h"
#include "FileIO.h"

class FileIOController
{
  public:
    FileIOController(FileIO *fileIO);
    bool SDExists(uint8_t cs_port = SS);
    void getDataFromConfigFile(FileIO fileIO);
    void writeDataToConfigFile();

  private:
    SdFat _SD;
    FileIO *_fileIO;
};

#endif //FILEIOCONTROLLER_H