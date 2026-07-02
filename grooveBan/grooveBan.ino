#include <Encoder.h>
#include <Bounce2.h>
#include <Audio.h>
#include <SD.h>
#include <SPI.h>

AudioRecordQueue     queue1;
AudioPlaySdWav       playFile; // To play back the loop after recording
File                 loopFile;
enum State { IDLE, RECORDING, PLAYING, PAUSE };
State loopState = IDLE;
// Define your pins
const int buttonPins[] = {2, 3, 4, 5, 6, 7, 8, 9, 17, 18, 19}; 
const int numButtons = 11;
Bounce buttons[numButtons];

// Define your encoders
Encoder enc1(26, 27);
Encoder enc2(28, 29);

/*****************************************************************************************
                                            Main Setup
*****************************************************************************************/
void setup(){
  buttonSetup();
}


/******************************************************************************************
                                          Main LOOP
********************************************************************************************/

void loop(){
  
}

/*
*****************************************************************************************                         Create an array of Bounce objects
                         Bounce buttons[numButtons];
************************************* *****************************************************/
void buttonSetup() {
  //buttomns 
  for (int i = 0; i < numButtons; i++) {
    buttons[i].attach(buttonPins[i], INPUT_PULLUP);
    buttons[i].interval(25); // Debounce interval
  }
  //sd card
  AudioMemory(10);
  // Initialize SD card on Shield
  if (!(SD.begin(10))) { 
    Serial.println("SD Card Error");
  }
}

void buttonLoop() {
  for (int i = 0; i < numButtons; i++) {
    buttons[i].update(); // Update all buttons
    
    if (buttons[i].fell()) {
      handleButtonPress(i); // Pass the index (0-10) to a handler
    }
  }
}

void handleButtonPress(int index) {
  // Use a switch statement to trigger different sounds based on index
  switch(index) {
    case 0: triggerKick(); break;
    case 1: triggerSnare(); break;
    // ... and so on
  }
}

/*****************************************************************************************
                                            The Looper (Recoder )
*****************************************************************************************/

void toggleRecord() {
  if (loopState == IDLE) {
    // Start Recording
    loopFile = SD.open("LOOP.RAW", FILE_WRITE);
    queue1.begin();
    loopState = RECORDING;
  } else if (loopState == RECORDING) {
    // Stop Recording
    queue1.end();
    loopFile.close();
    loopState = IDLE;
  }
}

void playLoop() {
  if (loopState == IDLE) {
    playFile.play("LOOP.RAW");
    loopState = PLAYING;
  }
}

void processAudio() {
  if (loopState == RECORDING && queue1.available() >= 1) {
    byte* buffer = (byte*)queue1.readBuffer();
    loopFile.write(buffer, 256);
    queue1.freeBuffer();
  }
}

/*****************************************************************************************
  The Drum Kit (Using SD Card)
*****************************************************************************************/



// Audio Objects
AudioPlaySdWav       kickDrum; // Kick
AudioPlaySdWav       Snare; // Snare
AudioMixer4          mixer1;
AudioOutputI2S       i2s1;

// Connections
AudioConnection      patchCord1(kickDrum, 0, mixer1, 0);
AudioConnection      patchCord2(Snare, 0, mixer1, 1);
AudioConnection      patchCord3(mixer1, 0, i2s1, 0);
AudioConnection      patchCord4(mixer1, 0, i2s1, 1);



// Call this function when you press the button
// Logic for triggering a sound
void triggerKick() {
  // If the player is already playing, we decide: 
  // Should we stop it and restart (re-trigger) or ignore?
  // For a drum machine, we usually want to re-trigger immediately.
  kickDrum.stop(); 
  kickDrum.play("KICK.WAV");
}

void triggerSnare() {
  Snare.stop();
  Snare.play("SNARE.WAV");
}


/*************************************************************************
                
**********************************/
