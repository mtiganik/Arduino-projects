/***************************************************************************
    Project: UI part 2
    Author: Mihkel Tiganik
    Date: 30.07.20183 
    
  *****Hardware******
  1) OLED display
  2) Micro SD card with adapter module
  3) Joystick
  4) photoresistor for reading analog data

  Create function that records list (100-200 items)
  of values with 10-20 Hz frequency. Create new file and save it to SD card
  After selecting created file from list, it must be opened as full screen
  chart, where horizontal axes is time(counter) and vertical is scaled values
 
***************************************************************************/

#include <Adafruit_GFX.h>      // For OLED SSD1306 display
#include <Adafruit_SSD1306.h>  // For OLED SSD1306 display
#include <SPI.h>               // For SD card
#include <SdFat.h>                // For SD card
//#include "SDIO.h"

#define SD_CS_PORT             SS
#define DEBOUNCE               10
#define DEFAULT_DELAY_TIME     10 // should be greater or equal then debounce
#define MAX_GRAPH_POINTS      500
#define SENSOR_PIN             A2
#define JOYSTICK_X_PIN         A0
#define JOYSTICK_Y_PIN         A1
#define JOYSTICK_SW_PIN         8
#define LEFT                    0
#define UP                      1
#define RIGHT                   2
#define DOWN                    3
#define PUSH                    4
#define NUMBUTTONS              5
#define MAX_FREQUENCY          30
//#define NEW_FILE_NAME_LENGTH  7

#define mainMenuItemsCount      3
SdFat SD;
int recordFrequency; // only between 10 ... 20
int* recordFrePtr;
double recordPeriod;               // will be declared in initializeNewGraph(). Takes DEFAULT_DELAY_TIME into account
unsigned long timeNow = 0;

char fileNameNoSubFix[6];
char fullFileName[20];

char configFile[] = "config.txt";
String mainMenuItems[] = {
  "Start new",
  "Show graph",
  "Settings"
};
#define settingsMenuItemsCount  5

String settingsMenuItems[5] {
  "Frequency",
  "Point count",
  "Line type",
  "Scrolling",
  "File name"
};

enum States {
  mainDisplay,
  newGraph,
  newGraphStop,
  showGraph,
  settings,
  settingsFrequency,
  settingsPointCnt,
  settingsLineType,
  settingsScrolling,
  settingsFileName
};

enum graphDisplay {
  fullScreen = 0,
  scrolling  = 1
};
graphDisplay currentgraphDisplay;

States currentState;
int selectedIndex = 0;

double graphValues[MAX_GRAPH_POINTS];
//double graphSmoothedAverageValue;
double graphMinValue;
double graphMaxValue;
double graphStep;
double averageValue;
double maxMinDifference ;
int numOfGraphPoints;
int currentGraphPoint = 0;
int scrollingModeMaxScrollValue;
int scrollerCurrentValue;


int newFileCharSelector = 0;
int frequencySelectorDigit = 0;
int frequencySelectorDigitValue = 0;

bool lineMode;
bool showSaveOption = false;
bool chooseSaveYes = true;

byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];
Adafruit_SSD1306 display(4);

//SDIO sdio;
void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.setTextSize(1);
  display.clearDisplay();
  pinMode(JOYSTICK_SW_PIN, INPUT);
  digitalWrite(JOYSTICK_SW_PIN, HIGH);
  if (!SD.begin(SD_CS_PORT)) {
    Serial.println("initialization failed!");
    return;
  }
  readConfigFile();
  updateMenu(mainMenuItems, 0, "Main menu", mainMenuItemsCount);
  currentState = mainDisplay;
}

void loop() {

  check_switches();


  if (justreleased[UP])
  {
    if (currentState == mainDisplay) {
      if (selectedIndex == 0) {
        selectedIndex = mainMenuItemsCount - 1;
      } else {
        selectedIndex--;
      }
      updateMenu(mainMenuItems, selectedIndex, "Main menu", mainMenuItemsCount);
    }
    if (currentState == settings) {
      if (selectedIndex == 0) {
        selectedIndex = settingsMenuItemsCount - 1;
      } else {
        selectedIndex--;
      }
      updateMenu(settingsMenuItems, selectedIndex, "Settings", settingsMenuItemsCount);
    }
    if (currentState == settingsFrequency) {
      //frequencySelectorDigitValue++;
      displayFrequencySettings(frequencySelectorDigit, recordFrequency + 1 );
    }
    if (currentState == settingsFileName) {
      char tempChar1 = fileNameNoSubFix[newFileCharSelector];
      tempChar1--;
      writeNewFileMenu(newFileCharSelector, tempChar1);
    }
  }
  if (justreleased[DOWN])
  {
    if (currentState == mainDisplay) {
      if (selectedIndex == mainMenuItemsCount - 1) {
        selectedIndex = 0;
      } else {
        selectedIndex++;
      }
      updateMenu(mainMenuItems, selectedIndex, "Main menu", mainMenuItemsCount);
    }
    if (currentState == settings) {
      if (selectedIndex == settingsMenuItemsCount - 1) {
        selectedIndex = 0;
      } else {
        selectedIndex++;
      }
      updateMenu(settingsMenuItems, selectedIndex, "Settings", settingsMenuItemsCount);
    }
    if (currentState == settingsFrequency) {
      //frequencySelectorDigitValue--;
      displayFrequencySettings(frequencySelectorDigit, recordFrequency - 1 );
    }
    if (currentState == settingsFileName) {
      char tempChar2 = fileNameNoSubFix[newFileCharSelector];
      tempChar2++;
      writeNewFileMenu(newFileCharSelector, tempChar2);
    }

  }
  if (justreleased[LEFT]) {
    if (currentState == settings) {
      currentState = mainDisplay;
      selectedIndex = 2;
      updateMenu(mainMenuItems, selectedIndex, "Main menu", mainMenuItemsCount);
    } else if (currentState == showGraph) {
      scrollerCurrentValue--;
      if (scrollerCurrentValue >= 0) {
        displayFinalGraph(scrollerCurrentValue);
      } else if (scrollerCurrentValue == -2) {
        currentState = mainDisplay;
        selectedIndex = 1;
        updateMenu(mainMenuItems, selectedIndex, "Main menu", mainMenuItemsCount);
      }
    } else if (currentState == settingsFrequency) {
      if (frequencySelectorDigit == 0) {
        selectedIndex = 0;
        currentState = settings;
        updateMenu(settingsMenuItems, selectedIndex, "Settings", settingsMenuItemsCount);
      } else {
        frequencySelectorDigit--;
        displayFrequencySettings(frequencySelectorDigit, frequencySelectorDigitValue);
      }
    } else if (currentState == settingsFileName) {
      if (newFileCharSelector == 0) {
        currentState = settings;
        selectedIndex = 4;
        updateMenu(settingsMenuItems, selectedIndex, "Settings", settingsMenuItemsCount);
      } else {
        newFileCharSelector--;
        writeNewFileMenu(newFileCharSelector, fileNameNoSubFix[newFileCharSelector]);
      }
    }

  }
  if (justreleased[RIGHT]) {
    if (currentState == showGraph) {
      if (scrollerCurrentValue < scrollingModeMaxScrollValue) {
        scrollerCurrentValue++;
      }
      displayFinalGraph(scrollerCurrentValue);
    } else if (currentState == settingsFrequency) {
      if (frequencySelectorDigit == 1) {
        currentState = settings;
        selectedIndex = 0;
        if (saveNewConfigData()) {
          updateMenu(settingsMenuItems, selectedIndex, "Settings saved", settingsMenuItemsCount);
        } else {
          updateMenu(settingsMenuItems, selectedIndex, "Error saving data", settingsMenuItemsCount);
        }

        //updateMenu(settingsMenuItems, selectedIndex, "Settings", settingsMenuItemsCount);
      } else {
        frequencySelectorDigit++;
        displayFrequencySettings(frequencySelectorDigit, recordFrequency);
      }
    } else if (currentState == settingsFileName) {
      newFileCharSelector++;
      if(newFileCharSelector > sizeof(fileNameNoSubFix)){
        currentState = settings;
        selectedIndex = 4;
        if (saveNewConfigData()) {
          updateMenu(settingsMenuItems, selectedIndex, "Name modified", settingsMenuItemsCount);
        } else {
          updateMenu(settingsMenuItems, selectedIndex, "Error saving data", settingsMenuItemsCount);
        }
      }else{
         writeNewFileMenu(newFileCharSelector, fileNameNoSubFix[newFileCharSelector]);

      }
    }
    else {
      openDirectory(selectedIndex);

    }
  }
  if (justreleased[PUSH]) {
    Serial.println("PUSH");
  }

  if (currentState == newGraph) {
    if (millis() > timeNow + recordPeriod) {
      timeNow = millis();
      getNewGraphData();
    }
  }
  if (currentState == newGraphStop) {
    // if pressed any button after showing final graph, then open Save yes/No option
    if (!showSaveOption && (justreleased[LEFT] || justreleased[RIGHT])) {
      showSaveOption = true;
      chooseSaveYes = true;
      displaySaveOption(chooseSaveYes);
    }
    if (showSaveOption && (justreleased[LEFT] || justreleased[RIGHT])) {
      chooseSaveYes = !chooseSaveYes;
      displaySaveOption(chooseSaveYes);
    }
    if (showSaveOption && (justreleased[UP] || justreleased[DOWN] || justreleased[PUSH])) {
      currentState = mainDisplay;
      if (chooseSaveYes) {
        if (saveData()) {
          updateMenu(mainMenuItems, selectedIndex, "Saved succesfully", mainMenuItemsCount);
        } else {
          updateMenu(mainMenuItems, selectedIndex, "Error saving data", mainMenuItemsCount);
        }
      } else {
        updateMenu(mainMenuItems, selectedIndex, "Didnt save", mainMenuItemsCount);

      }
    }

  }
  // 10 ms default delay to saturate check_switches function
  delay(DEFAULT_DELAY_TIME);
}


bool saveNewConfigData() {
  Serial.println(recordFrequency);
  Serial.println(numOfGraphPoints);
  Serial.println(lineMode);
  Serial.println(currentgraphDisplay);
  Serial.println(fileNameNoSubFix);
  char outPutter[20];
  if (!SD.begin(SD_CS_PORT)) {
    Serial.println("Can't find SD!");
    return false;
  }

  if (SD.exists(configFile)) {
    SD.remove(configFile);
  }
  File conf = SD.open(configFile, FILE_WRITE);
  if (conf) {
    conf.print("FILENAME=");
    conf.println(fileNameNoSubFix);
    conf.print("FREQ=");
    conf.println(recordFrequency);
    conf.print("POINT_CNT=");
    conf.println(numOfGraphPoints);
    conf.print("LINE=");
    conf.println((int)lineMode);
    conf.print("SCROLLING=");
    conf.println((int)currentgraphDisplay);
    conf.close();
    return true;
  } else {
    return false;
  }

}


bool saveData() {
readConfigFile();
  if (!SD.begin(SD_CS_PORT)) {
    Serial.println("initialization failed!");
    return false;
  }

  if (SD.exists(fullFileName)) {
    SD.remove(fullFileName);
  }

  File dataFile = SD.open(fullFileName, FILE_WRITE);
  if (dataFile) {
    dataFile.print("FREQ=");
    dataFile.println(recordFrequency);
    int counter = 0;
    while (counter < numOfGraphPoints) {
      dataFile.println(graphValues[counter]);
      counter++;
    }
    dataFile.close();
    return true;
  } else {
    return false;
  }
}



void displaySaveOption(bool yesSelected) {
  display.setCursor(30, 20);
  display.setTextColor(WHITE);
  display.print("Save data?");
  display.setCursor(33, 30);
  display.fillRect(33, 30, 48, 8, BLACK);
  if (yesSelected) {
    display.setTextColor(BLACK, WHITE);
    display.print("Yes");
    display.setTextColor(WHITE);
    display.print(" No");
  } else {
    display.setTextColor(WHITE);
    display.print("Yes ");
    display.setTextColor(BLACK, WHITE);
    display.print("No");
  }
  display.display();
}

void getNewGraphData() {
  //Serial.println(analogRead(SENSOR_PIN));
  double currentGraphValue = analogRead(SENSOR_PIN);

  graphValues[currentGraphPoint] = currentGraphValue;

  //  if (currentGraphValue > graphMaxValue) {
  //    graphMaxValue = currentGraphValue;
  //  } else if ( currentGraphValue < graphMinValue) {
  //    graphMinValue = currentGraphValue;
  //  }
  //graphSmoothedAverageValue = (graphSmoothedAverageValue * (currentGraphPoint -1) + currentGraphValue) / currentGraphPoint;

  if (currentGraphPoint < numOfGraphPoints) {
    updateNewGraphPlot();
    currentGraphPoint++;
  } else {
    //currentgraphDisplay = fullScreen; // Leave it fullscreen to disable any scrolling.

    initializeFinalGraph();
    displayFinalGraph(0);
    showSaveOption = false; // reinitialize it to false so after measurement you can see data
    currentState = newGraphStop;
  }
}

void initializeFinalGraph() {
  averageValue = 0;
  graphMaxValue = graphValues[0];
  graphMinValue = graphValues[0];
  for (int i = 0; i < numOfGraphPoints; i++) {
    averageValue += graphValues[i];
    if (graphValues[i] > graphMaxValue) {
      graphMaxValue = graphValues[i];
    } else if (graphValues[i] < graphMinValue) {
      graphMinValue = graphValues[i];
    }
  }
  averageValue = averageValue / numOfGraphPoints;

  maxMinDifference = graphMaxValue - graphMinValue;

  // reinitialize graphStep variable
  if (currentgraphDisplay == fullScreen) {
    graphStep = (double)128 / numOfGraphPoints;
  } else {
    graphStep = 1;
    scrollingModeMaxScrollValue = numOfGraphPoints - 120 > 0 ? (numOfGraphPoints - 120) / 10 : 1;
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("MIN");
  display.print((int)graphMinValue);
  display.print(" MAX");
  display.print((int)graphMaxValue);
  display.print(" AVG");
  display.print((int)averageValue);
  display.drawLine(0, 8, 0, 64, WHITE);
  display.drawLine(0, 63, 124, 63, WHITE);
  display.display();
}

void displayFinalGraph(int scroller) {
  display.fillRect(1, 8, 124, 55, BLACK);
  //  int iMin = (currentgraphDisplay == fullScreen) ? 0 : scroller * 10;
  //  int iMax = (currentgraphDisplay == fullScreen) ? numOfGraphPoints : (scroller + 12) * 10;
  int iMin = 0;
  int iMax = (currentgraphDisplay == fullScreen) ? numOfGraphPoints : 120;

  for (int i = iMin; i < iMax; i++) {
    int xPos = i * graphStep;
    int yPos = 64 - (((currentgraphDisplay == fullScreen) ? graphValues[i] : graphValues[scroller * 10 + i]) - graphMinValue ) / maxMinDifference * 54;
    display.drawPixel(xPos, yPos, WHITE);
  }

  display.display();

}

void initializeNewGraph() {
  currentGraphPoint = 0;


  double initialGraphValue = analogRead(SENSOR_PIN);
  // get some initial min and max values for graph
  graphMinValue = initialGraphValue - 10;
  graphMaxValue = initialGraphValue + 10;
  // graphSmoothedAverageValue = initialGraphValue;

  graphValues[currentGraphPoint] = initialGraphValue;


  // to get frequency correct with main loop delay() time.
  if (1000 / recordFrequency > DEFAULT_DELAY_TIME) {
    recordPeriod = 1000 / recordFrequency - DEFAULT_DELAY_TIME;
  } else {
    recordPeriod = 0;
  }

  initializeNewGraphPlot();
  updateNewGraphPlot();
  currentGraphPoint++;
}

void initializeNewGraphPlot() {
  graphStep = (double)128 / numOfGraphPoints;
  display.clearDisplay();
  display.drawLine(0, 0, 0, 64, WHITE);
  display.drawLine(0, 63, 124, 63, WHITE);
  display.display();
}

void updateNewGraphPlot() {
  //  Serial.print("graphStep: ");
  //  Serial.println(graphStep);
  int xPos = (currentGraphPoint) * graphStep;
  int yPos = 64 - graphValues[currentGraphPoint] / 16;
  display.drawPixel(xPos, yPos, WHITE);
  display.fillRect(5, 0, 22, 7, BLACK);
  display.setCursor(5, 0);
  display.print(currentGraphPoint);
  display.display();

}

void displayFrequencySettings(int selectedIndex, int selectedFrequency) {
  Serial.print("Freq before: "); Serial.println(recordFrequency);

  if (selectedIndex == 0) {
    if (selectedFrequency <= MAX_FREQUENCY && selectedFrequency > 0) {
      recordFrequency = selectedFrequency;
    }
  }

  Serial.print("Freq after: "); Serial.println(recordFrequency);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("Set Frequency");
  display.println();
  //  if (selectedIndex == 0) {
  //    display.setTextColor(BLACK, WHITE);
  //  }
  display.print(recordFrequency / 10);
  display.setTextColor(WHITE);
  if (selectedIndex == 0) {
    display.setTextColor(BLACK, WHITE);
  }
  display.print(recordFrequency % 10);
  display.setTextColor(WHITE);

  display.println(" Hz");
  display.print("       ");
  if (selectedIndex == 1) {
    display.setTextColor(BLACK, WHITE);
  }
  display.print("SAVE");
  display.setTextColor(WHITE);
  display.display();

}


void openDirectory(int selector) {

  // logic for opening main menu items
  if (currentState == mainDisplay) {
    if (mainMenuItems[selector] == "Start new") {

      initializeNewGraph();
      currentState = newGraph;
    }
    if (mainMenuItems[selector] == "Show graph") {
      if (!getDataValuesFromSD()) {
        updateMenu(settingsMenuItems, selectedIndex, "Didnt find SD", settingsMenuItemsCount);
      } else {
        currentgraphDisplay = scrolling;
        initializeFinalGraph();
        scrollerCurrentValue = scrollingModeMaxScrollValue;
        displayFinalGraph(scrollerCurrentValue);
        currentState = showGraph;
      }
    }
    if (mainMenuItems[selector] == "Settings") {
      selectedIndex = 0;
      updateMenu(settingsMenuItems, selectedIndex, "Settings", settingsMenuItemsCount);
      currentState = settings;
    }
  }
  // logic for opening settings menu items
  if (currentState == settings) {
    if (settingsMenuItems[selector] == "Frequency") {
      currentState = settingsFrequency;
      frequencySelectorDigit = 0;
      displayFrequencySettings(frequencySelectorDigit, recordFrequency);
    }
    if (settingsMenuItems[selector] == "Point count") {
      // TODO: add logic for Point count select box
    }
    if (settingsMenuItems[selector] == "Line type") {
      // TODO: add logic for Line type select box
    }
    if (settingsMenuItems[selector] == "Scrolling") {
      // TODO: add logic for Scrolling select box
    }
    if (settingsMenuItems[selector] == "File name") {
      currentState = settingsFileName;
      newFileCharSelector = 0;
      writeNewFileMenu(0, fileNameNoSubFix[0]);

      // TODO: add logic for Save file name select box
    }
  }

}

void writeNewFileMenu(int selectedChar, char selectedCharValue) {

  char charValue = selectedCharValue;

  // modify newfileNameNoSubFix character value
  if (selectedChar < 4) {
    if (charValue == 0x40) {
      charValue = 0x5A;
    } else if (charValue == 0x5B) {
      charValue = 0x41;
    }
  } else {
    if (charValue == 0x2F) {
      charValue = 0x39;
    } else if (charValue == 0x3A) {
      charValue = 0x30;
    }
  }
  fileNameNoSubFix[selectedChar] = charValue;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("Modify file name");
  display.println("Insert name");
  display.println();

  for (int i = 0; i < sizeof(fileNameNoSubFix); i++) {
    if (i == selectedChar) {
      display.setTextColor(BLACK, WHITE);
    } else {
      display.setTextColor(WHITE);
    }
    display.print(fileNameNoSubFix[i]);
  }
  display.setTextColor(WHITE);
  display.println(".txt");
  display.println();
  display.print("         ");
  if (selectedChar >= sizeof(fileNameNoSubFix)) {
    display.setTextColor(BLACK, WHITE);
  }
  display.print("SAVE");
  display.display();

}

bool getDataValuesFromSD() {
  if (!SD.begin(SD_CS_PORT)) {
    Serial.println("initialization failed!");
    return false;
  }
  Serial.println(fullFileName);
  File dataFile = SD.open(fullFileName, FILE_READ);
  if (dataFile) {
    char charGetter;
    char lineGetter[100]; // Line should be small in this file
    int charCounter = 0;
    int lineCounter = 0;
    while (dataFile.available()) {
      charGetter = dataFile.read();

      lineGetter[charCounter] = charGetter;
      charCounter++;
      if (charGetter == '\n') {
        if (lineCounter == 0) {
          char* separator = strchr(lineGetter, '=');
          if (separator != 0) {
            *separator = 0;
            ++separator;
            recordFrequency = atoi(separator);
          }
        }
        else {
          graphValues[lineCounter - 1] = atoi(lineGetter);
        }
        lineCounter++;
        charCounter = 0;
      }
    }
    numOfGraphPoints = lineCounter - 1;
    dataFile.close();
    return true;
  } else {
    return false;
  }

}


void readConfigFile() {
  if (!SD.begin(SD_CS_PORT)) {
    Serial.println("initialization failed!");
    return;
  }

  File conf = SD.open(configFile, FILE_READ);
  if (conf) {
    char charGetter;
    char fileData[200]; // Line should be small in this file
    int charCounter = 0;
    int lineCounter = 0;
    while (conf.available()) {
      charGetter = conf.read();
      fileData[charCounter] = charGetter;
      charCounter++;
    }
    conf.close();
    char* line = strtok(fileData, "\n");
    while (line != 0) {
      char* separator = strchr(line, '=');
      if (separator != 0) {
        *separator = 0;

        if (strcmp(line, "FREQ") == 0) {
          ++separator;
          recordFrequency = atoi(separator);
          // atoi(separator);

        } else if (strcmp(line, "POINT_CNT") == 0) {
          ++separator;
          numOfGraphPoints = atoi(separator);

        } else if (strcmp(line, "LINE") == 0) {
          ++separator;
          lineMode = atoi(separator);
        } else if (strcmp(line, "SCROLLING") == 0) {
          ++separator;
          if (atoi(separator) == 1) {
            currentgraphDisplay = scrolling;
          } else {
            currentgraphDisplay = fullScreen;
          }
        }
        else if (strcmp(line, "FILENAME") == 0) {
          ++separator;
          strcpy(fileNameNoSubFix, separator);
        }
      }
      line = strtok(0, "\n");
    }
  }

  String fullFileName0 = fileNameNoSubFix;
  fullFileName0[6] = '.';
  String fullFileName1 = fullFileName0 + "txt"; //
  for (int i = 0; i < 10; i++) {
    fullFileName[i] = fullFileName1[i];
  }
  fullFileName[10] = 0;
  // Serial.println(fullFileName);
  // Serial.println(recordFrequency);
  // Serial.println(numOfGraphPoints);
  // Serial.println(fileNameNoSubFix);
}


void updateMenu(String menuItems[], int selectedIndex, String menuTitle, int numberOfItems) {
  display.clearDisplay();
  display.setCursor(10, 1);
  display.setTextColor(WHITE);
  display.println(menuTitle);
  display.drawLine(0, 8, 123, 8, WHITE);
  if (numberOfItems < 5) {
    display.println();
  }
  for (int i = (selectedIndex < 6 ? 0 : selectedIndex - 5); i < numberOfItems; i++) {
    if (i == selectedIndex) {
      display.setTextColor(BLACK, WHITE);
      display.print(">|");
    } else {
      display.setTextColor(WHITE);
      display.print(" |");
    }
    display.println(menuItems[i]);
  }
  display.display();
}

void check_switches()
{
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;
  if (millis() < lasttime) {
    lasttime = millis(); // we wrapped around, lets just try again
  }

  if ((lasttime + DEBOUNCE) > millis()) {
    return; // not enough time has passed to debounce
  }
  delay(10);
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();


  // currentstate[PUSH]  = analogRead(JOYSTICK_SW_PIN) < 30  ? 0 : 1;   // read the button
  currentstate[LEFT]  = analogRead(JOYSTICK_X_PIN)  < 100 ? 0 : 1;
  currentstate[UP]    = analogRead(JOYSTICK_Y_PIN)  < 100 ? 0 : 1;
  currentstate[RIGHT] = analogRead(JOYSTICK_X_PIN)  > 800 ? 0 : 1;
  currentstate[DOWN]  = analogRead(JOYSTICK_Y_PIN)  > 800 ? 0 : 1;
  currentstate[PUSH]  = digitalRead(JOYSTICK_SW_PIN);
  for (index = 0; index < NUMBUTTONS; index++) {
    justpressed[index] = 0;       // when we start, we clear out the "just" indicators
    justreleased[index] = 0;

    // currentstate[index] = digitalRead(buttons[index]);   // read the button
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        // just released
        justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }
    //Serial.println(pressed[index], DEC);
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }

}

