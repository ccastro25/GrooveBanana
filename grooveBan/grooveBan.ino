#include <Encoder.h>
#include <Bounce2.h>
#include <Audio.h>
#include <SD.h>
#include <SPI.h>

// --- Audio Objects ---
AudioInputI2S        i2sInput; 
AudioRecordQueue     queue1;
AudioPlaySdWav       playFile;
AudioPlaySdWav       kickDrum;
AudioPlaySdWav       Snare;
AudioMixer4          mixer1;
AudioOutputI2S       i2s1;

// --- Audio Connections ---
AudioConnection      patchCord1(kickDrum, 0, mixer1, 0);
AudioConnection      patchCord2(Snare, 0, mixer1, 1);
AudioConnection      patchCord3(i2sInput, 0, queue1, 0); // Connect Input to Queue
AudioConnection      patchCord4(mixer1, 0, i2s1, 0);
AudioConnection      patchCord5(mixer1, 0, i2s1, 1);

File                 loopFile;
enum State { IDLE, RECORDING, PLAYING, PAUSE };
State loopState = IDLE;

const int buttonPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 17, 18, 19}; 
const int numButtons = 11;
Bounce buttons[numButtons];

Encoder enc1(26, 27);
Encoder enc2(28, 29);

void setup() {
  Serial.begin(9600);
  buttonSetup();
  AudioMemory(10);
  if (!(SD.begin(10))) { 
    Serial.println("SD Card Error");
  }
}

void loop() {
  buttonLoop();
  processAudio();
}

void buttonSetup() {
  for (int i = 0; i < numButtons; i++) {
    buttons[i].attach(buttonPins[i], INPUT_PULLUP);
    buttons[i].interval(25);
  }
}

void buttonLoop() {
  for (int i = 0; i < numButtons; i++) {
    buttons[i].update();
    if (buttons[i].fell()) {
      handleButtonPress(i);
    }
  }
}

void handleButtonPress(int index) {
  switch(index) {
    case 0: triggerKick(); break;
    case 1: triggerSnare(); break;
    case 2: toggleRecord(); break;
  }
}

void triggerKick() {
  kickDrum.stop(); 
  kickDrum.play("KICK.WAV");
}

void triggerSnare() {
  Snare.stop();
  Snare.play("SNARE.WAV");
}

void toggleRecord() {
  if (loopState == IDLE) {
    SD.remove("LOOP.RAW");
    loopFile = SD.open("LOOP.RAW", FILE_WRITE);
    queue1.begin();
    loopState = RECORDING;
  } else if (loopState == RECORDING) {
    queue1.end();
    loopFile.close();
    loopState = IDLE;
  }
}

void processAudio() {
  if (loopState == RECORDING && queue1.available() >= 1) {
    byte* buffer = (byte*)queue1.readBuffer();
    loopFile.write(buffer, 256);
    queue1.freeBuffer();
  }
}