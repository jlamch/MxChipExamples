#include "RGB_LED.h"
#include "Arduino.h"

// led related
RGB_LED rgbLed;
unsigned int rgbColour[3];
bool runLed = true;

void setup() {
  //start led
  rgbLed.setColor(0, 0, 255);

  // attach buttons monitoring functions
  attachInterrupt(USER_BUTTON_A, startLedFunc, CHANGE);
  attachInterrupt(USER_BUTTON_B, startLedFunc, CHANGE);
}

void loop() {
  delay(1000);
  rgbLed.turnOff();
  delay(2000);

  if(runLed)
  {
    ledChangingProgram();
    runLed = false;
  }
}

void startLedFunc()
{
    runLed = true;
}

void ledChangingProgram()
{
  int32_t countFrom = 4;
  for(int32_t iter = countFrom; iter >= 0; iter--) {
    // Start off with red.
    rgbColour[0] = 255;
    rgbColour[1] = 0;
    rgbColour[2] = 0;
    // Choose the colours to increment and decrement.
    for (int decColour = 0; decColour < 3; decColour += 1) {
      int incColour = decColour == 2 ? 0 : decColour + 1;

      // cross-fade the two colours.
      for(int i = 0; i < 255; i += 1) {
          rgbColour[decColour] -= 1;
          rgbColour[incColour] += 1;

          rgbLed.setColor(rgbColour[0], rgbColour[1], rgbColour[2]);
          delay(3);
      }
    }
  }
}