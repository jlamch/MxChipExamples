#define NOISEBUFFER_SIZE 40

#include "Arduino.h"
#include "OledDisplay.h"
#include "AudioClassV2.h"
#include "RingBuffer.h"


AudioClass& Audio = AudioClass::getInstance();
const int AUDIO_SIZE = 32000 * 3 + 45;

int lastButtonAState;
int buttonAState;
int lastButtonBState;
int buttonBState;
char *audioBuffer;
int totalSize;
int monoSize;

//noise level
uint16_t noiseBuffer[NOISEBUFFER_SIZE];
uint16_t noiseBufferSend[NOISEBUFFER_SIZE];
float noiseRms = 0.0;
float noiseLog = 0.0;
float noiseSmoothed = 0.0;

void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  Serial.println("Helloworld in Azure IoT DevKits!");

  // initialize the button pin as a input
  pinMode(USER_BUTTON_A, INPUT);
  lastButtonAState = digitalRead(USER_BUTTON_A);
  pinMode(USER_BUTTON_B, INPUT);
  lastButtonBState = digitalRead(USER_BUTTON_B);

  // Setup your local audio buffer
  audioBuffer = (char *)malloc(AUDIO_SIZE + 1);
  memset(audioBuffer, 0x0, AUDIO_SIZE);

  
  memset(noiseBuffer, 0x0, sizeof(noiseBuffer));
  memset(noiseBufferSend, 0x0, sizeof(noiseBufferSend));
}


void loop(void)
{
  printIdleMessage();

  while (1)
  {
    buttonAState = digitalRead(USER_BUTTON_A);
    buttonBState = digitalRead(USER_BUTTON_B);
    
    if (buttonAState == LOW && lastButtonAState == HIGH)
    {
      record();
    }

    if (buttonBState == LOW && lastButtonBState == HIGH)
    {
      play();
    }
    
    lastButtonAState = buttonAState;
    lastButtonBState = buttonBState;
  }

  delay(10);
}

void printIdleMessage()
{
  Screen.clean();
  Screen.print(0, "AZ3166 Audio:  ");
  Screen.print(1, "Press A to record", true);
  Screen.print(2, "Press B to play", true);
}

void record()
{
  // Re-config the audio data format
  Audio.format(8000, 16);
  Audio.setVolume(80);

  Serial.println("start recording");
  Screen.clean();
  Screen.print(0, "Start recording");

  // Start to record audio data
  Audio.startRecord(audioBuffer, AUDIO_SIZE);

  // Check whether the audio record is completed.
  while (digitalRead(USER_BUTTON_A) == LOW && Audio.getAudioState() == AUDIO_STATE_RECORDING)
  {
    delay(10);
  }
  Audio.stop();
  
  Screen.clean();
  Screen.print(0, "Finish recording");
  totalSize = Audio.getCurrentSize();
  Serial.print("Recorded size: ");
  Serial.println(totalSize);
  
  Screen.clean();
  char out [100];
  sprintf(out, "Finish recording %d",totalSize);
  Screen.print( out, true); 

  // noise level
  CalcLoudness(totalSize);
  PrintNoiseData(0);


  delay(3000);  
  printIdleMessage();
}

void play()
{
  Screen.clean();
  Screen.print(0, "Start playing");
  Audio.startPlay(audioBuffer, totalSize);
  
  while (Audio.getAudioState() == AUDIO_STATE_PLAYING)
  {
    delay(10);
  }
  
  Screen.print(0, "Stop playing");
  printIdleMessage();
}


///noise level
float squareSum = 0.0;
int k = 1;
int block = 0;
void CalcLoudness(int length)
{
  for (int i = 0; i < length; i = i + 2)
  {
    short value = (short)audioBuffer[i] | (short)audioBuffer[i + 1] << 8;

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
