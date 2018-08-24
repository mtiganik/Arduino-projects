/***************************************************************************
    Project: Environment sensors and logging
    Author: Mihkel Tiganik
    Date: 25.07.2018 
    
  *****Hardware******
  1) Dallas 18B20 digital temperature sensor - port 2. 1kOhm resistor between port 2 and +5V
  2) DHT 11 digital environmental sensor - port 11
  3) BOSH BME 280 environmental sensor - I2C interface. Address 0x76
  4) Photoresistor: 5V - Photoresistor - 1 kOhm - GND. Connect A0 between photoresistor and resistor
  5) SD CARD - Catalex Micro SD. See http://www.vishnumaiea.in/projects/hardware/interfacing-catalex-micro-sd-card-module
    SD Card connected:
    CS -> SS pin 53
    SCK -> SCK pin 52
    MOSI -> MOSI pin 51
    MISO -> MISO pin 50
    VCC -> 5V
  6) Real time Clock DS1302RTC. https://playground.arduino.cc/Main/DS1302RTC

  ******Task***** 
  Connect all sensors, OLED display, SD card and clock with Arduino. 
  Display sensors data values on OLED display in every 10 seconds.
  in every 5 minutes write average measured values to SD card.

 ***************************************************************************/

#include <Wire.h>              // For BME 280
#include <SPI.h>               // For BME 280 
#include <Adafruit_Sensor.h>   // For BME 280
#include <Adafruit_BME280.h>   // For BME 280
#include <Adafruit_GFX.h>      // For OLED SSD1306 display
#include <Adafruit_SSD1306.h>  // For OLED SSD1306 display
#include <OneWire.h>           // For DS18B20
#include <DallasTemperature.h> // For DS18B20
#include <dht.h>               // For DHT11
#include <SPI.h>               // For SD card
#include "SdFat.h"             // For SD card
#include <stdio.h>             // For RTC DS1303
#include <DS1302.h>            // For RTC DS1303

SdFat SD;                      // Replace SD libary with SdFat

#define DHT11_PIN         7    // connect DHT sensor with pin 7
#define DS18B20_BUS       2    // connect DS18B20 with pin 2
#define PHOTORESISTOR_PIN A0   // connect: +5V, photoresistor , 1kOhm, GND. 
// pin A0 between photoresistor and 1kOhm
#define OLED_RESET 4

#define SD_CS_PIN SS           // SD CS pin to SS (Pin 53 on MEGA)

#define CLOCK_CE_PIN      27   // CE / RST / Chip select pin 27
#define CLOCK_DATA_PIN    29   // Clock data to pin 29
#define CLOCK_CLK_PIN     31   // Clock CLK pin 31       

#define SD_CARD_WRITE_INTERVAL 5 // Write to SD card in every 5 mins

// Object initialization
DS1302 rtc(CLOCK_CE_PIN, CLOCK_DATA_PIN, CLOCK_CLK_PIN);
dht DHT;
Adafruit_BME280 bme; // I2C
OneWire oneWire(DS18B20_BUS);
DallasTemperature DS18B20(&oneWire); // Pass our oneWire reference to Dallas Temperature.
Adafruit_SSD1306 display(OLED_RESET);
File sensorDataFile;

unsigned long delayTime = 10000;
long photoResistorValue = 0;
char defaultFileName[] = "SensorData.csv";

// Variables to store temp measurement results
String currentDate;
String currentTime;
double DS18B20temp;
double DHTtemp;
double DHThumidity;
double BMEtemp;
double BMEhumidity;
double BMEpressure;
double lightness;
// Variables to store average measurement data
double DS18B20tempAVG = 0;
double DHTtempAVG     = 0;
double DHThumidityAVG = 0;
double BMEtempAVG     = 0;
double BMEhumidityAVG = 0;
double BMEpressureAVG = 0;
double lightnessAVG   = 0;
int readingCountBeforeAvg = 0;
int currentMinutes;
int previousMinutes = 50;
void setup() {
  
  Serial.begin(9600);

  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS_PIN)) {

    Serial.println("Initialization of SD Card failed.");
    Serial.println("Check connections");
    return;
  }
  Serial.println("initialization done.");

  //SensorDataFile = SD.open(defaultFileName);
  // If there is no such file on SD card
  if (!SD.exists(defaultFileName)) {

    sensorDataFile = SD.open(defaultFileName, FILE_WRITE);
    if (sensorDataFile) {
      // write initial header row to csv file
      sensorDataFile.println("Date, Time, 18B20 temp, DHT temp, DHT humidity, BME temp, BME humidity, BLE pressure, Lightness \r\n");
      sensorDataFile.close();
      Serial.println("New file created");
    }

  } else {
    Serial.println("File exists");
  }

 

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  bool status;

  // default settings for BME280
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin();
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  // Start up DallasTemperature Libary
  DS18B20.begin();

  // // THIS IS FOR NEW CLOCK TIME
  // // UNCOMMENT BELOW IF NEW CLOCK TIME IS NEEDED
  // // BY DEFAULT IT SHOULD BE COMMENTED
  // rtc.writeProtect(false);
  // rtc.halt(false);

  // // Make a new time object to set the date and time.
  //Time t(2018, 7, 24, 15, 51, 50, Time::kTuesday);

  // // Set the time and date on the chip.
  //rtc.time(t);
}


void loop() {
  getValues();
  //printValuesToSerial();  // comment this if you want to get data for longer time

  // if longer time (5 minutes) have passed, write values to SD card
  if(abs(currentMinutes - previousMinutes ) > SD_CARD_WRITE_INTERVAL){
    getAverageValues();
    printValuesToSD();
    resetAverageValues();
    previousMinutes = currentMinutes;
  }
  // Show current values on OLED display
  printValuesToDisplay();

  // delay 10 seconds to get new data
  delay(delayTime);
}

// reset average values after they have ben written to SD
void resetAverageValues(){
  DS18B20tempAVG = 0;
  DHTtempAVG     = 0;
  DHThumidityAVG = 0;
  BMEtempAVG     = 0;
  BMEhumidityAVG = 0;
  BMEpressureAVG = 0;
  lightnessAVG   = 0;
  readingCountBeforeAvg = 0;
}

void getAverageValues(){
  DS18B20tempAVG = DS18B20tempAVG / readingCountBeforeAvg;
  DHTtempAVG     = DHTtempAVG / readingCountBeforeAvg;
  DHThumidityAVG = DHThumidityAVG / readingCountBeforeAvg;
  BMEtempAVG     = BMEtempAVG / readingCountBeforeAvg;
  BMEhumidityAVG = BMEhumidityAVG / readingCountBeforeAvg;
  BMEpressureAVG = BMEpressureAVG / readingCountBeforeAvg;
  lightnessAVG   = lightnessAVG / readingCountBeforeAvg;
}

void printValuesToSD() {

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Can't find SD card");
    return;
  }

  sensorDataFile = SD.open(defaultFileName, FILE_WRITE);
  if (sensorDataFile) {

    sensorDataFile.print(currentDate);
    sensorDataFile.print(",");
    sensorDataFile.print(currentTime);
    sensorDataFile.print(",");
    sensorDataFile.print(DS18B20tempAVG);
    sensorDataFile.print(",");
    sensorDataFile.print(DHTtempAVG);
    sensorDataFile.print(",");
    sensorDataFile.print(DHThumidityAVG);
    sensorDataFile.print(",");
    sensorDataFile.print(BMEtempAVG);
    sensorDataFile.print(",");
    sensorDataFile.print(BMEhumidityAVG);
    sensorDataFile.print(",");
    sensorDataFile.print(BMEpressureAVG);
    sensorDataFile.print(",");
    sensorDataFile.print(lightnessAVG);
    sensorDataFile.print("\r\n");

    sensorDataFile.close();
    Serial.print(currentTime);
    Serial.println("  data added to SD");
  }


}

void printValuesToDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("18B20 temp,  C: ");
  display.println(DS18B20temp);
  display.print("DHT temp,    C: ");
  display.println(DHTtemp);
  display.print("DHT Humidity %: ");
  display.println(DHThumidity);
  display.print("BME temp,    C: ");
  display.println(BMEtemp);
  display.print("BME humidity %: ");
  display.println(BMEhumidity);
  display.println("BME pressure, hPa: ");
  display.print("            ");
  display.println(BMEpressure);
  display.print("Lightness,   %: ");
  display.println(lightness);

  display.display();
}

// for debbugging purposes
void printValuesToSerial() {
  Serial.print("Date: ");
  Serial.print(currentDate);
  Serial.print(" ");
  Serial.println(currentTime);
  Serial.print("DS18B20 temperature: ");
  Serial.println(DS18B20temp);
  Serial.print("DHT temperature: ");
  Serial.println(DHTtemp);
  Serial.print("DHT Humidity: ");
  Serial.println(DHThumidity);
  Serial.print("BME temperature: ");
  Serial.println(BMEtemp);
  Serial.print("BME humidity, %: ");
  Serial.println(BMEhumidity);
  Serial.print("BME pressure, hPa: ");
  Serial.println(BMEpressure);
  Serial.print("Lightness, %: ");
  Serial.println(lightness);
  Serial.println();
}

void getValues() {
  currentDate = GetDate();
  currentTime = GetTime();
  currentMinutes = GetCurrentMinutes();
  DS18B20.requestTemperatures();
  int dhtChk = DHT.read11(DHT11_PIN);
  DS18B20temp  = DS18B20.getTempCByIndex(0); // By index because you can have more than one sensor on same bus
  DHTtemp      = DHT.temperature;
  DHThumidity  = DHT.humidity;
  BMEtemp      = bme.readTemperature();
  BMEhumidity  = bme.readHumidity();
  BMEpressure  = bme.readPressure();
  double sensorValue = analogRead(PHOTORESISTOR_PIN);
  lightness    = sensorValue * 100 / 1023;

  DS18B20tempAVG += DS18B20temp;
  DHTtempAVG     += DHTtemp;
  DHThumidityAVG += DHThumidity;
  BMEtempAVG     += BMEtemp;
  BMEhumidityAVG += BMEhumidity;
  BMEpressureAVG += BMEpressure;
  lightnessAVG   += lightness;
  readingCountBeforeAvg++;
}

int GetCurrentMinutes(){
   Time t = rtc.time();
   return t.min;
}

String GetDate(){
   Time t = rtc.time();
  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
           t.yr, t.mon, t.date);
  return buf;
}

String GetTime() {
  // Get the current time and date from the clock chip.
  Time t = rtc.time();

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
           t.hr, t.min, t.sec);
  return buf;
}
