/*
    Project: Snake game with 8x8 LED display
    Author: Mihkel Tiganik
    Date: 19.07.2018
    Hardware used:
      Arduino MEGA board
      2 pushbuttons
      MAX7218 8x8 LED display
      LED for showing game status
    Description: Create Snake game with MAX7218 console
    Snake game has 2 buttons, clockwise and counterClockWise
    Buttons. 
*/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

int pinCS = 10; // attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numberOfHorizontalDisplays = 1;
int numberOfVerticalDisplays = 1;

static unsigned long last_interrupt_time;
unsigned long interrupt_time;

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

const int width = 8;
const int height = 8;

int MAP[width][height] = {};
bool running;
bool ButtonPressed = false;
signed int ButtonPressCounter = 1002;
int direction;
int headxpos;
int headypos;
int food; // initial snake length

void setup() {
  // put your setup code here, to run once:
  matrix.setIntensity(0);
  attachInterrupt(2, CounterClcWise_Interrupt, FALLING); // pin 21
  attachInterrupt(3, ClockWise_Interrupt, FALLING);     // pin 20
  headxpos = 0;
  headypos = 4;
  food     = 3;
  running = true;
  direction = 0;
  pinMode(12, OUTPUT);    // connect LED to pin 12, which will show if game is over

  digitalWrite(12, LOW);
  MAP[headxpos][headypos] = food;
  randomSeed(analogRead(0));
  generateFood();
}

void loop() {
  while (running) {


    if (ButtonPressed) {
      ButtonPressed = false;
      changeDirection();
    }

    updateMap();

    // clear screen
    matrix.fillScreen(LOW);

    // print screen
    printScreen();
    delay(200);
  }
  // GAME OVER. Press reset on MEGA for new game
  digitalWrite(12, HIGH);
}

void generateFood() {
  int x = 0;
  int y = 0;
  do {
    x = random(8);
    y = random(8);
  } while (MAP[x][y] != 0);

  //place new food
  MAP[x][y] = -2;
}

void printScreen() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {

      matrix.drawPixel(i, j, (MAP[i][j] == 0 ? LOW : HIGH));
    }
  }
  matrix.write();
}

void updateMap() {
  switch (direction) {
    case 0: move(1, 0);  // rigth. Test it
      break;
    case 1: move(0, -1); //up
      break;
    case 2: move(-1, 0); // left
      break;
    case 3: move(0, 1);  // down
      break;
  }

  // reduce snake values on map by 1
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      if (MAP[i][j] > 0) MAP[i][j]--;
    }
  }
}
void move(int dx, int dy) {
  int newx = headxpos + dx;
  int newy = headypos + dy;

  // check if there was food
  if (MAP[newx][newy] == -2) {
    // increase body length
    food++;
    // generate new food to map
    generateFood();
  } 
  else if (newx > width || newy > height || MAP[newx][newy] != 0)
  {
    running = false;
  }
  // check if location was valid

  headxpos = newx;
  headypos = newy;
  MAP[headxpos][headypos] = food + 1;

}

void changeDirection() {
  switch (ButtonPressCounter % 4) {
    case (0):
      //move rigth
      direction = 0;
      break;
    case (1):
      // move up
      direction = 1;
      break;
    case (2):
      // move left
      direction = 2;
      break;
    case (3):
      direction = 3;
      break;
  }
}

void CounterClcWise_Interrupt() {
  //last_interrupt_time = 0;
  interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 100) {
    ButtonPressCounter++;
    ButtonPressed = true;
  }
  last_interrupt_time = interrupt_time;
}

void ClockWise_Interrupt() {

  interrupt_time = millis();

  if (interrupt_time - last_interrupt_time > 100) {
    ButtonPressCounter--;
    ButtonPressed = true;
  }
  last_interrupt_time = interrupt_time;
}


