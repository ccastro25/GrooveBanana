#include <Encoder.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

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

// --- Patch Cords (Ensure these match your Design Tool export) ---
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
Encoder enc2(28, 29);
const int buttonPins[] = {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40};
const int numButtons = 11;
unsigned long lastDebounceTime[11] = {0};
const unsigned long debounceDelay = 250; // Shortened for responsiveness

// --- Variables ---
float volume = 0.5;
long oldPos = -999;

void setup() {
  Serial.begin(9600);
  AudioMemory(250); // Increased for stability with multiple players
  
  sgtl5000_1.enable();
  sgtl5000_1.volume(volume);
  
  if (!(SD.begin(BUILTIN_SDCARD))) {
    Serial.println("SD Card Error");
    while(1); 
  }

  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  Serial.println("Setup Complete");
}

void loop() {
  handleEncoder();
  handleButtons();
}

void handleEncoder(){
  long newPos = enc2.read() / 4; // Dividing by 4 for typical encoder detents
  if (newPos != oldPos) {
    if (newPos > oldPos) volume += 0.05;
    else volume -= 0.05;
    
    volume = constrain(volume, 0.0, 1.0);
    sgtl5000_1.volume(volume);
    oldPos = newPos;
    Serial.print("Volume: "); Serial.println(volume);
  }
}

void handleButtons() {
  unsigned long currentMillis = millis();
  for (int i = 0; i < numButtons; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
       if (currentMillis - lastDebounceTime[i] > debounceDelay) {
          triggerInstrument(i);
          lastDebounceTime[i] = currentMillis; 
       }
    }
  }
}
void ChangeKey() {
  long newPos = enc2.read();
  Serial.print("this is encouder: "+ newPos);
  int delta = (newPos - lastEncPos) / 4; 
  if (delta != 0) {
    currentKeyIndex = (currentKeyIndex + delta) % 12;
    if (currentKeyIndex < 0) currentKeyIndex += 12;
    lastEncPos = newPos;
    Serial.print("Key: "); 
    Serial.println(keyNames[currentKeyIndex]);
  }
void triggerInstrument(int index) {
  Serial.println("triggers index :");
  Serial.println(index);
  switch(index) {
    case 3: pianoSdWav6.play("C.wav"); break;
    case 4: PianoSdWav9.play("D.wav"); break;
    case 5: PianoSdWav10.play("E.wav"); break;
    case 6: PianoSdWav7.play("F.wav"); break;
    case 7: KickSDWay.play("G.wav"); break;
    case 8: SnareSdWav2.play("A.wav"); break;
    case 9: HigHatSdWav3.play("B.wav"); break;
    case 10: HigHatSdWav3.play("C1.wav"); break;
  }
}
/*
#include <Encoder.h>
#include <Metro.h>

// Initialize encoder on pins 5 and 6
Encoder myEnc(5, 6);

// Set initial BPM and constraints
int currentBpm = 120;
const int MIN_BPM = 40;
const int MAX_BPM = 240;

// Set up the metronome timer
long interval = 60000 / currentBpm;
Metro metronome = Metro(interval);

// Track the previous encoder position
long oldPosition  = -999;

void setup() {
  pinMode(9, OUTPUT); // Click output pin
  Serial.begin(9600);
}

void loop() {
  // 1. Read encoder position (divide by 4 for standard 4-step encoders)
  long newPosition = myEnc.read() / 4;
  
  if (newPosition != oldPosition) {
    // Calculate new BPM based on encoder movement direction
    int change = newPosition - oldPosition;
    currentBpm += change;
    
    // Keep BPM within safe musical limits
    currentBpm = constrain(currentBpm, MIN_BPM, MAX_BPM);
    
    // Update the metronome timing interval instantly
    long newInterval = 60000 / currentBpm;
    metronome.interval(newInterval);
    
    // Print updated tempo to Serial Monitor
    Serial.print("Tempo: ");
    Serial.concat(currentBpm);
    Serial.println(" BPM");
    
    oldPosition = newPosition;
  }

  // 2. Fire the metronome click
  if (metronome.check()) {
    digitalWrite(9, HIGH);
    delay(20); // Short pulse for a crisp click sound
    digitalWrite(9, LOW);
  }
}
*/