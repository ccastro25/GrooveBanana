#include <Encoder.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav           KickSDWay;     //xy=179,434
AudioPlaySdWav           BassSdWav4;     //xy=182,481
AudioPlaySdWav           HigHatSdWav3;     //xy=185,381
AudioPlaySdWav           SnareSdWav2;     //xy=190,331
AudioPlaySdWav           pianoSdWav6;     //xy=192,614
AudioPlaySdWav           PianoSdWav9;     //xy=199,747
AudioPlaySdWav           PianoSdWav10;    //xy=203,814
AudioPlaySdWav           PianoSdWav7;     //xy=204,657
AudioMixer4              Pianomixer3;         //xy=385,746
AudioMixer4              Bassmixer4;         //xy=407,450
AudioMixer4              Pianomixer1;         //xy=410,642
AudioMixer4              mixer2;         //xy=431,329
AudioMixer4              mixer5;         //xy=632,538
AudioOutputI2S           i2s1;           //xy=837,482
AudioConnection          patchCord1(KickSDWay, 0, Bassmixer4, 0);
AudioConnection          patchCord2(KickSDWay, 1, Bassmixer4, 1);
AudioConnection          patchCord3(BassSdWav4, 0, Bassmixer4, 2);
AudioConnection          patchCord4(BassSdWav4, 1, Bassmixer4, 3);
AudioConnection          patchCord5(HigHatSdWav3, 0, mixer2, 2);
AudioConnection          patchCord6(HigHatSdWav3, 1, mixer2, 3);
AudioConnection          patchCord7(SnareSdWav2, 0, mixer2, 0);
AudioConnection          patchCord8(SnareSdWav2, 1, mixer2, 1);
AudioConnection          patchCord9(pianoSdWav6, 0, Pianomixer1, 0);
AudioConnection          patchCord10(pianoSdWav6, 1, Pianomixer1, 1);
AudioConnection          patchCord11(PianoSdWav9, 0, Pianomixer3, 0);
AudioConnection          patchCord12(PianoSdWav9, 1, Pianomixer3, 1);
AudioConnection          patchCord13(PianoSdWav10, 0, Pianomixer3, 2);
AudioConnection          patchCord14(PianoSdWav10, 1, Pianomixer3, 3);
AudioConnection          patchCord15(PianoSdWav7, 0, Pianomixer1, 2);
AudioConnection          patchCord16(PianoSdWav7, 1, Pianomixer1, 3);
AudioConnection          patchCord17(Pianomixer3, 0, mixer5, 3);
AudioConnection          patchCord18(Bassmixer4, 0, mixer5, 1);
AudioConnection          patchCord19(Pianomixer1, 0, mixer5, 2);
AudioConnection          patchCord20(mixer2, 0, mixer5, 0);
AudioConnection          patchCord21(mixer5, 0, i2s1, 0);
AudioConnection          patchCord22(mixer5, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=432,159
// GUItool: end automatically generated code

// 1. Setup Encoders
Encoder enc1(26, 27);
Encoder enc2(28, 29);

// 2. Setup Buttons
const int buttonPins[] = {30, 31, 32, 33, 34, 35, 36, 37, 38, 40, 41};
const int numButtons = 11;

// Timing variables for non-blocking debounce
unsigned long lastDebounceTime[11] = {0}; 
const unsigned long debounceDelay = 50; // 50ms is usually enough for buttons

long pos1 = -999, pos2 = -999;

void setup() {
  Serial.begin(9600);
  Serial.print("setting up ");
  // Set all buttons as input with internal pull-up
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  // Always check inputs
  handleEncoders();
  //handleButtons();
}

void handleEncoders() {
  long newPos1 = enc1.read();
  Serial.print(" enconder "+ newPos1);
  if (newPos1 != pos1) {
    pos1 = newPos1;
    // USE THIS: Map encoder 1 to change a parameter (e.g., volume or pitch)
  }

  long newPos2 = enc2.read();
  if (newPos2 != pos2) {
    pos2 = newPos2;
    // USE THIS: Map encoder 2 to change a different parameter (e.g., effect intensity)
  }
}

void handleButtons() {
  unsigned long currentMillis = millis();

  for (int i = 0; i < numButtons; i++) {
    if (digitalRead(buttonPins[i]) == LOW) { // Button pressed
      if (currentMillis - lastDebounceTime[i] > debounceDelay) {
        
        // --- TRIGGER LOGIC ---
        // You can use a switch(i) statement here to map specific pins to specific sounds
        triggerInstrument(i);
        
        lastDebounceTime[i] = currentMillis;
      }
    }
  }
}

void triggerInstrument(int index) {
  // Example: Pins 30-35 are Keyboard, 36-41 are Drums
  if (index < 6) {
    // Play Note logic
  } else {
    // Play Drum Sample logic
  }
}