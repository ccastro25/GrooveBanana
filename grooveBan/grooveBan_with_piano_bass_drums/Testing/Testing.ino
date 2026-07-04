#include <Encoder.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
//SD.open("looper/loop1.wav", FILE_WRITE)
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

// --- Patch Cords ---
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
const unsigned long debounceDelay = 500;

// --- Variables ---
long oldPos =0;
long pos1 = -999;
int currentKeyIndex = 0;
long lastEncPos = 0;
const char* keyNames[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
/*
  1 skip an inex 
  2 skip an index
  3 go to the next index
  4 skip an index
  5 skip an index
  6 skip an inex 
  7 skip an index
  8 next index but octave


  
*/
void setup() {

  Serial.begin(9600);
    Serial.println("start setup ");
  AudioMemory(16);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  if (!(SD.begin(BUILTIN_SDCARD))) {
    Serial.println("SD Card Error: Check if FAT32 formatted");
    while(1); 
  }

Serial.println("start Loop");
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  Serial.println("done setup");
}

void loop() {
  // put your main code here, to run repeatedly:
    long newPos = enc2.read();
    if(newPos != oldPos ){
        Serial.println("this is encouder: ");
        Serial.println(newPos);
        oldPos= newPos;
    }
   
  handleButtons();

}

void handleButtons() {

  unsigned long currentMillis = millis();


  for (int i = 0; i < numButtons; i++) {
        if (digitalRead(buttonPins[i]) == LOW){
      // Serial.println("inButt: ");
       if(currentMillis - lastDebounceTime[i] > debounceDelay){
          Serial.println("currentMillis: ");
          Serial.println(currentMillis);
         Serial.println("LastDebounceTime");
          Serial.println(lastDebounceTime[i]);
           Serial.println("DebounceDelay ");
           Serial.println(debounceDelay);

        Serial.println("finally ");
        lastDebounceTime[0] = currentMillis;
        triggerInstrument(i);
      lastDebounceTime[i] = currentMillis;
       }
      // Serial.println("inButt: ");

    }
    //if  (currentMillis - lastDebounceTime[i] > debounceDelay) {Serial.println("is biger");}
    if (digitalRead(buttonPins[i]) == LOW && (currentMillis - lastDebounceTime[i] > debounceDelay)) {
     // Serial.println("it does work");

    }
  }
}

void triggerInstrument(int index) {
  switch(index) {
    case 0: 
      pianoSdWav6.play("Instruments/piano/E.wav"); 
      Serial.println("Playing E note"); 
      break;
    case 1: 
      PianoSdWav9.play("Instruments/piano/C .wav"); // Note the space
      break;
    case 2: 
      PianoSdWav10.play("Instruments/piano/G.wav"); // Ensure casing matches
      break;
    case 3: 
      PianoSdWav7.play("Instruments/piano/B.wav");  // Assuming you fix the 'Bwav' typo
      break;
    case 6: 
      KickSDWay.play("Instruments/drums/kick.wav"); 
      break;
    case 7: 
      SnareSdWav2.play("Instruments/drums/snare.wav"); 
      break;
    case 8: 
      HigHatSdWav3.play("Instruments/drums/hi-hat.wav"); 
      break;
  }
}