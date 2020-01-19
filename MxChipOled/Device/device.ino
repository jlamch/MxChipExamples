#include "OledDisplay.h"

void setup() {
  //screen
  Screen.init();
}

void loop() {
   oledPlay();
}

void oledPlay()
{
  Screen.clean();
  Screen.print("1. Some long text that should go to next lines");
  delay(3000);

  Screen.clean();
  Screen.print("2. Some long text that should go to\nnext lines", true);
  delay(3000);

  Screen.clean();
  Screen.print(0, "3. short text");
  Screen.print(1, "splited in lines");
  Screen.print(2, "there is only");
  Screen.print(3, "3 lines. ");
  Screen.print(4, "to use ");
  delay(3000);

  Screen.clean();
  char out [300];
  sprintf(out, "ID: %s\r\n Humid: %d\r\n Temp: %d\r\n", 155, 0, 0);
  Screen.print( out, true);
  delay(3000);

  unsigned char BMP[1024] ; 
  for (size_t j = 0; j < 8; j++)
  {
    for (size_t i = 0; i < 128; i++)
    {
      int a = j*127 + i;
      if(a % 16 < 8)
      {
        if(j % 2 == 0 )
        { BMP[a] = 255; }
        else
        { BMP[a] = 0; }      
      }
      else{
        if(j%2 == 0 )
        { BMP[a] = 0; }
        else
        { BMP[a] = 255; }
      }
    }   
  }

  // draw a bitmap to the screen
  Screen.clean();
  Screen.draw(0, 0, 128, 8, BMP);
  delay(3000);
}