#ifndef UTILITY_H
#define UTILITY_H

void serializeMessage(char *payload, uint16_t* noiseData, int noiseDataSize, float temperature, float humidity);

void SensorInit(void);

void blinkLED(void);
void blinkSendConfirmation(void);
int getInterval(void);

float readTemperature();
float readHumidity();

#endif /* UTILITY_H */
