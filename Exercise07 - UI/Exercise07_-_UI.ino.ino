/***************************************************************************
    Project: UI
    Author: Mihkel Tiganik
    Date: 26.07.20183 
    
  *****Hardware******
  1) OLED display
  2) Micro SD card with adapter module
  3) Joystick
 

  ******Task***** 
  Create 8-10 different .txt files on SD card with some content
  Make user friendly selection of files from SD card that is using file list.
  If user selects document, then it's content is displayed on screen
  Long files will have scrollbar and scrolling feature
  Add additional items to menu, like creating new file, show bitmap images
  and change display brigthness to full / low


 ***************************************************************************/

#include <Adafruit_GFX.h>      // For OLED SSD1306 display
#include <Adafruit_SSD1306.h>  // For OLED SSD1306 display
#include <SPI.h>               // For SD card
#include <SD.h>                // For SD card

#define SD_CS_PORT             SS
#define DEBOUNCE               10
#define JOYSTICK_X_PIN         A0
#define JOYSTICK_Y_PIN         A1
#define JOYSTICK_SW_PIN        A2   // It is accually not used
#define PUSH                    0
#define LEFT                    1
#define UP                      2
#define RIGHT                   3
#define DOWN                    4
#define NUMBUTTONS              5   // number of joystick buttons
#define MAX_SD_FILE_CNT        32   // max number of files in SD card
#define MAX_FILE_NAME_SIZE     32
#define MAX_TXT_FILE_CHARS   2048   // max characters that will be displayed to OLED
#define MAX_TXT_FILE_ROWS     100
#define NEW_FILE_NAME_LENGTH    7   // New files names can be 6 chars long

// Modes for variable currentMode
#define directoryMode   0
#define txtFileMode     1
#define pictureMode     2
#define brigthnessMode  3
#define newFileMode     4

int currentMode;

char newFileName[NEW_FILE_NAME_LENGTH] = "TEXT00";

// TODO: make picture adding more scalable
String availablePictures[2] = {
  "house",
  "tree"
};

#define NUM_OF_PICTURES 2

const unsigned char PROGMEM tree [] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00,
  0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x06, 0x61, 0x80, 0x00,
  0x20, 0x00, 0x07, 0xF1, 0x80, 0x00, 0x70, 0x00, 0x01, 0xF1, 0x80, 0x00, 0xF0, 0x00, 0x01, 0xF1,
  0x80, 0x00, 0xF8, 0x00, 0x01, 0xF9, 0x80, 0x01, 0xFC, 0x00, 0x01, 0x99, 0x80, 0x01, 0xFE, 0x00,
  0x01, 0x81, 0x80, 0x07, 0xFF, 0x80, 0x00, 0x01, 0x80, 0x0F, 0xFB, 0xF0, 0x00, 0x01, 0x80, 0x3E,
  0xF8, 0xFF, 0x80, 0x01, 0x81, 0xF8, 0xF8, 0x1F, 0xC0, 0x01, 0x87, 0xE0, 0xF8, 0x00, 0x00, 0x01,
  0x8F, 0xC0, 0xF8, 0x00, 0x00, 0x01, 0x8F, 0x01, 0xF8, 0x00, 0x00, 0x01, 0x88, 0x03, 0xFE, 0x00,
  0x00, 0x01, 0x80, 0x07, 0xFF, 0x80, 0x00, 0x01, 0x80, 0x1E, 0xFB, 0xC0, 0x00, 0x01, 0x80, 0x3C,
  0xF8, 0xF0, 0x00, 0x01, 0x80, 0x78, 0xFC, 0x7F, 0x00, 0x01, 0x87, 0xE3, 0xFC, 0x1F, 0x80, 0x01,
  0x8F, 0xCF, 0xDE, 0x00, 0x00, 0x01, 0x8C, 0x3E, 0xFF, 0x00, 0x00, 0x01, 0x80, 0x78, 0xFF, 0xC0,
  0x00, 0x01, 0x80, 0x00, 0xF7, 0xF0, 0x00, 0x01, 0x80, 0x03, 0xF3, 0x7E, 0x00, 0x01, 0x80, 0x0F,
  0xFB, 0xDF, 0xE0, 0x01, 0x80, 0x7F, 0xF9, 0xFF, 0xC0, 0x01, 0x8F, 0xFF, 0xF8, 0x3F, 0x80, 0x01,
  0x9F, 0xF9, 0xFC, 0x01, 0xC0, 0x01, 0x80, 0x00, 0xFF, 0x00, 0x00, 0x01, 0x80, 0x01, 0xFF, 0x80,
  0x00, 0x01, 0x80, 0x07, 0xFF, 0xE0, 0x00, 0x01, 0x80, 0x7F, 0xF7, 0xFF, 0xC0, 0x01, 0x87, 0xFE,
  0xF3, 0xFF, 0xE0, 0x01, 0x8F, 0xE1, 0xF8, 0x3F, 0xE0, 0x01, 0x80, 0x03, 0xFC, 0x00, 0x00, 0x01,
  0x80, 0x03, 0xEE, 0x00, 0x00, 0x01, 0x80, 0x07, 0xFF, 0x00, 0x00, 0x01, 0x80, 0x03, 0xFF, 0xC0,
  0x00, 0x01, 0x80, 0x07, 0xFF, 0xE0, 0x00, 0x01, 0x80, 0x7F, 0xFF, 0xFB, 0x07, 0x8D, 0xBF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

const unsigned char PROGMEM house [] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80,
  0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80, 0x07, 0x80, 0x00, 0x01, 0x80, 0x04,
  0x80, 0x00, 0x01, 0x80, 0x04, 0x80, 0x00, 0x01, 0x80, 0x04, 0x80, 0x00, 0x01, 0x80, 0x7F, 0xFE,
  0x00, 0x01, 0x80, 0xC0, 0x02, 0x00, 0x01, 0x80, 0x80, 0x03, 0x00, 0x01, 0x81, 0x00, 0x01, 0x00,
  0x01, 0x83, 0x00, 0x01, 0x80, 0x01, 0x82, 0x00, 0x00, 0xC0, 0x01, 0x84, 0xFF, 0xC3, 0xC0, 0x01,
  0x87, 0x80, 0x7E, 0x80, 0x01, 0x84, 0x00, 0x00, 0x80, 0x01, 0x84, 0x00, 0x00, 0x80, 0x01, 0x84,
  0xF8, 0x00, 0x80, 0x01, 0x84, 0x88, 0x00, 0x80, 0x01, 0x84, 0x88, 0x00, 0x80, 0x01, 0x84, 0xB8,
  0x7C, 0x80, 0x01, 0x84, 0xE8, 0x44, 0x80, 0x01, 0x84, 0x00, 0x44, 0x80, 0x01, 0x84, 0x00, 0x44,
  0x80, 0x01, 0x84, 0x00, 0x44, 0x83, 0xC1, 0x84, 0x00, 0x44, 0x82, 0x41, 0x84, 0x00, 0x00, 0x86,
  0x41, 0x80, 0x00, 0x00, 0x04, 0x41, 0x80, 0x00, 0x00, 0x07, 0xC1, 0x80, 0x00, 0x00, 0x00, 0x01,
  0x80, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80,
  0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00,
  0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

int selectedIndex      = 0;
int writeIndex         = 0;
int txtSelectedIndex   = 0;
int txtFileCharCount   = 0;   
int txtLineCounter     = 0;
int txtNumOfCharsInRows[MAX_TXT_FILE_ROWS];
int newFileCharSelector = 0;

File root;
File file;
byte switch_button     = HIGH;
byte currentBrigthness = HIGH;

String fileList[MAX_SD_FILE_CNT];
char txtFileContent[MAX_TXT_FILE_CHARS];
const uint8_t chipSelect = SS;


byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];

Adafruit_SSD1306 display(4);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.setTextSize(1);
  display.clearDisplay();
  Serial.begin(9600);

  if (!SD.begin(SD_CS_PORT)) {
    Serial.println("initialization failed!");
    return;
  }

  //SD.ls();
  root = SD.open("/");
  getDirectory(root);
  root.close();

  updateMenu(0);

  // Set UI for directory
  currentMode = directoryMode;
}




void loop() {
  check_switches();      // when we check the switches we'll get the current state

  if (justreleased[UP] ) {
    if (currentMode == directoryMode) {
      if (selectedIndex == 0) {
        selectedIndex = writeIndex - 1;
      } else {
        selectedIndex--;
      }
      updateMenu(selectedIndex);
    }
    if (currentMode == brigthnessMode) {
      currentBrigthness = !currentBrigthness;
      writeBrigthnessMenuToDisplay(currentBrigthness);
    }
    if (currentMode == txtFileMode && txtSelectedIndex != 0) {
      txtSelectedIndex--;
      writeTxtToDisplay(selectedIndex, txtSelectedIndex);
    }
    if (currentMode == newFileMode) {
      char tempChar1 = newFileName[newFileCharSelector];
      tempChar1--;
      writeNewFileMenu(newFileCharSelector, tempChar1);
    }

  }
  if (justreleased[DOWN] ) {
    if (currentMode == txtFileMode && txtSelectedIndex != txtLineCounter - 6) {
      txtSelectedIndex++;
      writeTxtToDisplay(selectedIndex, txtSelectedIndex);
    }
    if (currentMode == brigthnessMode) {
      currentBrigthness = !currentBrigthness;
      writeBrigthnessMenuToDisplay(currentBrigthness);
    }

    if (currentMode == directoryMode) {
      if (selectedIndex == writeIndex - 1) {
        selectedIndex = 0;
      } else {
        selectedIndex++;
      }
      updateMenu(selectedIndex);
    }
    if (currentMode == newFileMode) {
      char tempChar2 = newFileName[newFileCharSelector];
      tempChar2++;
      writeNewFileMenu(newFileCharSelector, tempChar2);
    }
  }
  if (justreleased[RIGHT]) {
    if (currentMode != newFileMode) {
      openFile(selectedIndex);
    }
    else {
      newFileCharSelector++;
      writeNewFileMenu(newFileCharSelector, newFileName[newFileCharSelector]);
    }
  }

  if (justreleased[LEFT]) {
    if (currentMode != directoryMode && currentMode != newFileMode ) {
      currentMode = directoryMode;
      updateMenu(selectedIndex);
    }
    if (currentMode == newFileMode) {
      if (newFileCharSelector == 0) {
        currentMode = directoryMode;
        updateMenu(selectedIndex);
      } else {
        newFileCharSelector--;
        writeNewFileMenu(newFileCharSelector, newFileName[newFileCharSelector]);
      }
    }
  }
  delay(10);
}

void openFile(int fileIndex) {

  if (SD.exists(fileList[fileIndex])) {
    // default txt file opening
    file = SD.open(fileList[fileIndex], FILE_READ);
    Serial.println(fileList[fileIndex]);
    getFile(file);
    file.close();
    writeTxtToDisplay(fileIndex, 0);
    currentMode = txtFileMode;
  }
  else if ( fileList[fileIndex] == "tree" )
  {
    display.clearDisplay();
    display.drawBitmap(10, 10, tree, 48, 48, WHITE);
    display.display();
    currentMode = pictureMode;
  }
  else if ( fileList[fileIndex] == "house" )
  {
    display.clearDisplay();
    display.drawBitmap(10, 10, house, 40, 40, WHITE);
    display.display();
    currentMode = pictureMode;
  }
  else if (fileList[fileIndex] == "Brigthness") {
    writeBrigthnessMenuToDisplay(currentBrigthness);
    currentMode = brigthnessMode;
  } 
  else if (fileList[fileIndex] == "New file") {
    newFileCharSelector = 0;
    writeNewFileMenu(0, newFileName[0]);
    currentMode = newFileMode;
  }
}
void writeNewFileMenu(int selectedChar, char selectedCharValue) {
 
  char charValue = selectedCharValue;

  // modify newFileName character value
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
  newFileName[selectedChar] = charValue;

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("Create new file");
  display.println("Insert file name");
  display.println();

  for (int i = 0; i < sizeof(newFileName) - 1; i++) {
    if (i == selectedChar) {
      display.setTextColor(BLACK, WHITE);
    } else {
      display.setTextColor(WHITE);
    }
    display.print(newFileName[i]);
  }
  display.setTextColor(WHITE);
  display.println(".txt");
  display.println();
  display.print("         ");
  if (selectedChar >= sizeof(newFileName) - 1) {
    display.setTextColor(BLACK, WHITE);
  }
  display.print("SAVE");
  if (selectedChar == sizeof(newFileName)) {
    display.setTextColor(WHITE);
    // insert new file making here
    display.println();
    if (makeNewFile()) {
      display.print(newFileName);
      display.println(" succesfully created");
    } else {
      display.println("File already exists");
    }
  }
  if(selectedChar == sizeof(newFileName) +1 ){
    
     currentMode = directoryMode;
     updateMenu(selectedIndex);    
  }
  display.display();

}
bool makeNewFile() {

  String fileName0 = newFileName;
  String fileName = fileName0 + ".txt";
  Serial.println(fileName);
  File tempFile = SD.open(fileName, FILE_READ);
  if (tempFile) {
    tempFile.close();
    return false;
  }
  tempFile.close();
  tempFile = SD.open(fileName, FILE_WRITE);
  tempFile.close();
  return true;

}

void writeBrigthnessMenuToDisplay(byte brigthness) {
  display.clearDisplay();
  display.setCursor(10, 0);
  display.setTextColor(WHITE);
  display.println("Set brigthness");
  display.println();

  if (brigthness == LOW) {
    display.setTextColor(WHITE);
    display.println(" | HIGH");
    display.setTextColor(BLACK, WHITE);
    display.println(">| LOW");
    display.dim(HIGH);
  }
  else {
    display.setTextColor(BLACK, WHITE);
    display.println(">| HIGH");
    display.setTextColor(WHITE);
    display.println(" | LOW");
    display.dim(LOW);
  }
  display.display();
}

void writeTxtToDisplay(int selectedFileIndex, int scrollIndex) {
  int charCounter = 0;
  //int lineCounter = 0;
  display.clearDisplay();
  display.setCursor(10, 0);
  display.setTextColor(BLACK, WHITE);
  display.println(fileList[selectedFileIndex]);
  display.setTextColor(WHITE);

  if (txtLineCounter <= 7) {
    for (int i = 0; i < txtFileCharCount; i++) {
      display.print(txtFileContent[i]);
    }
  } else {
    for (int i = getScrollerStartCharIndex(scrollIndex); i < txtFileCharCount; i++) {
      display.print(txtFileContent[i]);
    }
    drawScrollbar(scrollIndex);
  }

  display.display();
}

int getScrollerStartCharIndex(int scrollerIndex) {
  int returnValue = 0;
  for (int i = 0; i < scrollerIndex; i++) {
    returnValue += txtNumOfCharsInRows[i];
  }
  return returnValue;
}

void drawScrollbar(int scroll) {
  double height = 7 * 54 / txtLineCounter;
  double yPos = (scroll) * 54 / txtLineCounter + 10;
  display.fillRect(122, yPos, 3, height, WHITE);
}

void getFile(File f) {
  txtFileCharCount = 0;
  txtLineCounter = 0;
  txtSelectedIndex = 0;
  char temp;
  int currentRowCharCount = 0;
  while (f.available()) {
    temp = file.read();
    txtFileContent[txtFileCharCount] = temp;
    txtFileCharCount++;
    currentRowCharCount++;
    if (temp == '\n') {
      txtNumOfCharsInRows[txtLineCounter] = currentRowCharCount;
      currentRowCharCount = 0;
      txtLineCounter++;
      continue;
    }
    if (currentRowCharCount % 21 == 20) {
      txtFileContent[txtFileCharCount] = '\n';
      txtFileCharCount++;
      txtNumOfCharsInRows[txtLineCounter] = currentRowCharCount + 1;
      currentRowCharCount = 0;
      txtLineCounter++;
    }
  }
}


void updateMenu(int scrollIndex) {
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  for (int i = (scrollIndex < 7 ? 0 : scrollIndex - 6); i < writeIndex; i++) {
    if (i != scrollIndex) {
      display.setTextColor(WHITE);
      display.print(" |");
    } else {
      display.setTextColor(BLACK, WHITE);
      display.print(">|");
    }

    display.println(fileList[i]);
  }
  display.display();
}

void getDirectory(File dir) {
  writeIndex = 0;
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {

      // Here we write additional items to menu
      // stuff that is not in SD Card

      // Add new file popup
      fileList[writeIndex] = "New file";
      writeIndex++;

      // Add pictures
      for (int i = 0; i < NUM_OF_PICTURES; i++) {
        fileList[writeIndex] = availablePictures[i];
        writeIndex++;
      }

      // Brigthness popup
      fileList[writeIndex] = "Brigthness";
      writeIndex++;

      break;
    }
  
    fileList[writeIndex] = entry.name();
    writeIndex++;
   
    entry.close();
  }
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
  currentstate[PUSH]  = analogRead(JOYSTICK_SW_PIN) < 30  ? 0 : 1;   // read the button
  currentstate[LEFT]  = analogRead(JOYSTICK_X_PIN)  < 100 ? 0 : 1;
  currentstate[UP]    = analogRead(JOYSTICK_Y_PIN)  < 100 ? 0 : 1;
  currentstate[RIGHT] = analogRead(JOYSTICK_X_PIN)  > 800 ? 0 : 1;
  currentstate[DOWN]  = analogRead(JOYSTICK_Y_PIN)  > 800 ? 0 : 1;
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
