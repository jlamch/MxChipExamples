#include "Arduino.h"
#include "OledDisplay.h"
#include "RingBuffer.h"
#include "AudioClassV2.h"

#include "config.h"
#include "utility.h"

static AudioClass &Audio = AudioClass::getInstance();
static int AUDIO_SIZE = 32000 * 3 + 45;

char readBuffer[AUDIO_CHUNK_SIZE];
int lastButtonAState;
int buttonAState;
int lastButtonBState;
int buttonBState;
static bool hasWifi = false;
static bool messageSending = true;
static bool showValues = true;

uint16_t noiseBuffer[NOISEBUFFER_SIZE];
uint16_t noiseBufferSend[NOISEBUFFER_SIZE];

static uint64_t lastSendTime;
static uint64_t displayValuesTime;

float noiseRms = 0.0;
float noiseLog = 0.0;
float noiseSmoothed = 0.0;

void setup(void)
{
  Screen.init();
  PrintTitle();
  Screen.print(1, "Init");

  Screen.print(2, " > Serial");
  Serial.begin(115200);
  Serial.println("Noise Cloud starting");

  Screen.print(2, " > IO Pins");
  pinMode(USER_BUTTON_A, INPUT);

  lastButtonAState = digitalRead(USER_BUTTON_A);

  lastSendTime = SystemTickCounterRead();
  displayValuesTime = SystemTickCounterRead();

  memset(noiseBuffer, 0x0, sizeof(noiseBuffer));
  memset(noiseBufferSend, 0x0, sizeof(noiseBufferSend));

  StartRecord();
}

uint loopCount = 0;
void loop(void)
{
  loopCount++;
  buttonAState = digitalRead(USER_BUTTON_A);

  if (buttonAState == LOW && lastButtonAState == HIGH)
  {
    Serial.println("A pressed");
    displayValuesTime = SystemTickCounterRead();
    PrintTitle();
    showValues = true;
  }

  lastButtonAState = buttonAState;

  if (showValues)
    if ((int)(SystemTickCounterRead() - displayValuesTime) >= 30000)
    {
      showValues = false;
      delay(10);
      Screen.clean();
    }
    else
    {
      PrintNoiseData(loopCount % 20);
    }

  delay(10);
}

void PrintTitle()
{
  Screen.clean();
  Screen.print(0, "  Noise Cloud");
}

bool firstRecord;
void StartRecord()
{
  Serial.println("start recording");
  firstRecord = true;
  Audio.format(8000, 16);
  Audio.startRecord(recordCallback);
}

void recordCallback(void)
{
  if (firstRecord)
  {
    firstRecord = false;
    return;
  }

  int length = Audio.readFromRecordBuffer(readBuffer, AUDIO_CHUNK_SIZE);
  CalcLoudness(length);
}

float squareSum = 0.0;
int k = 1;
int block = 0;

void CalcLoudness(int length)
{
  for (int i = 0; i < length; i = i + 2)
  {
    short value = (short)readBuffer[i] | (short)readBuffer[i + 1] << 8;

    squareSum += value * value;
    k++;
  }

  block++;
  if (block == 16)
  {
    noiseRms = sqrt(squareSum / k);
    noiseLog = log10(noiseRms);
    UpdateSmoothed(noiseLog);

    squareSum = 0;
    k = 1;
    block = 0;

    int idx = (int)(noiseLog * 10);
    idx = min(max(idx, 0), NOISEBUFFER_SIZE);
    noiseBuffer[idx]++;
  }
}

void UpdateSmoothed(float value)
{
  float smoothAlpha = 0.1;
  noiseSmoothed = (smoothAlpha * value) + ((1 - smoothAlpha) * noiseSmoothed);
}

char buf[100];
void PrintNoiseData(int i)
{
  if (i == 0)
  {
    sprintf(buf, "%7.1f", noiseLog);
    Screen.print(2, buf);
  }
  if (i == 1)
  {
    sprintf(buf, "%7.1f", noiseSmoothed);
    Screen.print(3, buf);
  }
}