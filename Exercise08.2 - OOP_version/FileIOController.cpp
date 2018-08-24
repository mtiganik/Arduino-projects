/// FILE NAME:      FileIOController.cpp
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        03.08.2018
/// MODIFIED:       03.08.2018

#include "FileIOController.h"

FileIOController::FileIOController(FileIO *fileIO)
{
    this -> _fileIO = fileIO;
}

bool FileIOController::SDExists(uint8_t cs_port)
{
  if(!_SD.begin(cs_port)){
        // If no SD card was found return false
        return false;
    }
    return true;
}

void FileIOController::getDataFromConfigFile(FileIO fileIO)
{
      File file = _SD.open(CONFIG_FILE, FILE_READ);
    if(file){
          char dataFileName[20];
          char charGetter;
          char fileData[200];
          int charCounter = 0;
          int lineCounter = 0;
          while (file.available())
          {
              charGetter = file.read();
              fileData[charCounter] = charGetter;
              charCounter++;
          }
          file.close();
          char *line = strtok(fileData, "\n");
          while (line != 0)
          {
              char *separator = strchr(line, '=');
              if (separator != 0)
              {
                  *separator = 0;
                  if (strcmp(line, "FREQ") == 0)
                  {
                      ++separator;
                      _fileIO -> setFreq(atoi(separator));
                  }
                  else if (strcmp(line, "POINT_CNT") == 0)
                  {
                      ++separator;
                      _fileIO -> setPointCount(atoi(separator));
                  }
                  else if (strcmp(line, "LINE") == 0)
                  {
                      ++separator;
                      _fileIO -> setLineMode(atoi(separator));
                  }
                  else if (strcmp(line, "SCROLLING") == 0)
                  {
                      ++separator;
                      _fileIO -> setScrolling(atoi(separator));
                      
                  }
                  else if (strcmp(line, "FILENAME") == 0)
                  {
                      ++separator;
                      // setDataFileName will come in end because of casting
                      strcpy(dataFileName, separator);
                      dataFileName[6] = 0;
                  }
              }
              line = strtok(0, "\n");
          }
          String fileNameNoSubFix0 = dataFileName;
          fileNameNoSubFix0[6] = '.';
          _fileIO->setDataFileName(fileNameNoSubFix0 + ".txt");
    }
}

void FileIOController::writeDataToConfigFile()
{
   if(_SD.exists(CONFIG_FILE)){
        _SD.remove(CONFIG_FILE);
    }
    File file = _SD.open(CONFIG_FILE, FILE_WRITE);
    if (file)
    {
        file.print("FILENAME=");
        file.println(_fileIO->getDataFileName());
        file.print("FREQ=");
        file.println(_fileIO->getFreq());
        file.print("POINT_CNT=");
        file.println(_fileIO->getPointCount());
        file.print("LINE=");
        file.println((int)_fileIO->getLineMode());
        file.print("SCROLLING=");
        file.println((int)_fileIO->getScrollingEnabled());
        file.close();
    }
}