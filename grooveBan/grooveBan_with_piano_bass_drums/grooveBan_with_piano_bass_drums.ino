#include <Encoder.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// --- Audio Objects ---
AudioPlaySdWav           KickSDWay;
AudioPlaySdWav           BassSdWav4;
AudioPlaySdWav           HigHatSdWav3;
AudioPlaySdWav           SnareSdWav2;
AudioPlaySdWav           pianoSdWav6;
AudioPlaySdWav           PianoSdWav9;
AudioPlaySdWav           PianoSdWav10;
AudioPlaySdWav           PianoSdWav7;
AudioMixer4              Pianomixer3;
AudioMixer4              Bassmixer4;
AudioMixer4              Pianomixer1;
AudioMixer4              mixer2;
AudioMixer4              mixer5;
AudioOutputI2S           i2s1;
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
AudioControlSGTL5000     sgtl5000_1;

// --- Hardware Inputs ---
Encoder enc1(26, 27);
Encoder enc2(28, 29);
const int buttonPins[] = {30, 31, 32, 33, 34, 35, 36, 37, 38, 40, 41};
const int numButtons = 11;
unsigned long lastDebounceTime[11] = {0};
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(9600);
  AudioMemory(16);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  
  if (!(SD.begin(BUILTIN_SDCARD))) {
    Serial.println("SD Card Error: Check connection/format");
  }

  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

void loop() {
  handleEncoders();
  handleButtons();
}

void handleEncoders() {
  // Add parameter logic (volume/pitch) here using enc1.read() and enc2.read()
}

void handleButtons() {
  unsigned long currentMillis = millis();
  for (int i = 0; i < numButtons; i++) {
    if (digitalRead(buttonPins[i]) == LOW && (currentMillis - lastDebounceTime[i] > debounceDelay)) {
      triggerInstrument(i);
      lastDebounceTime[i] = currentMillis;
    }
  }
}

void triggerInstrument(int index) {
  Serial.print("Triggering Button: "); Serial.println(index); // Debugging
  switch(index) {
    case 0: pianoSdWav6.play("P1.WAV"); break;
    case 1: PianoSdWav9.play("P2.WAV"); break;
    case 2: PianoSdWav10.play("P3.WAV"); break;
    case 3: PianoSdWav7.play("P4.WAV"); break;
    case 6: KickSDWay.play("KICK.WAV"); break;
    case 7: SnareSdWav2.play("SNARE.WAV"); break;
    case 8: HigHatSdWav3.play("HAT.WAV"); break;
    case 9: BassSdWav4.play("BASS.WAV"); break;
  }
}