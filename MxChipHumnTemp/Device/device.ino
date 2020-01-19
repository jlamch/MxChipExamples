#include "Arduino.h"
#include "HTS221Sensor.h"
#include "OledDisplay.h"

// humidity related
bool runHum = true;
DevI2C *i2c;
HTS221Sensor *sensorHnT;
float humidity = 0;
float temperature = 0;
unsigned char id;

void setup() {
  attachInterrupt(USER_BUTTON_A, startStopHumidityFunc, CHANGE);
  attachInterrupt(USER_BUTTON_B, startStopHumidityFunc, FALLING); 

  // humidity and temperature related
  i2c = new DevI2C(D14, D15);
  sensorHnT = new HTS221Sensor(*i2c);
    // init the sensor
  sensorHnT -> init(NULL);

  //screen
  Screen.init();
}

void loop() {
  if(runHum)
  {
    humidityFunc();
  }
}

void startStopHumidityFunc()
{
  runHum = !runHum;
  if(runHum){
    // enable
    sensorHnT -> enable();
  }
  else{
    // disable the sensor
    sensorHnT -> disable();
    Screen.clean();
  }
}

void humidityFunc()
{
    // reset
    sensorHnT -> reset();    
    humidity = 0;
    // get humidity
    sensorHnT -> getHumidity(&humidity);
    // get temperature
    sensorHnT -> getTemperature(&temperature);

    char out [300];
    sprintf(out, "Humid: %.1f\r\n Temp: %.2f\r\n",  humidity, temperature);
    Screen.print( out, true);

    delay(1000);
}