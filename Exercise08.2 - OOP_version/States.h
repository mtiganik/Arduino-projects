/// FILE NAME:      States.h
/// AUTHOR:         Mihkel Tiganik
/// CREATED:        02.08.2018
/// MODIFIED:       03.08.2018

#ifndef STATES_H
#define STATES_H

enum class States {
  mainDisplay,
  newGraph,
  newGraphStop,
  showGraph,
  config,
  configFrequency,
  configPointCnt,
  configLineType,
  configScrolling,
  configFileName
};

#endif //STATES_H