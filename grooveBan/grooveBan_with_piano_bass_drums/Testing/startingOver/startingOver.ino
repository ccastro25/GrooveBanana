#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Encoder.h>
#include <Bounce.h>
#include <Metro.h>
//volume 
//tempo
//mode
//key
//scale
//chord

Metro debugTimer = Metro(1000);

// ====================== STATIC MEMORY ======================
#define MAX_BUFFER_WORDS 11025 // Optimized size to fit safely within Teensy RAM DTCM limits
// 8 slots for your 8 scale buttons
unsigned int sampleBuffer[8][MAX_BUFFER_WORDS]; 

// ====================== AUDIO ======================
const int numPlayers = 6;
AudioPlayMemory players[numPlayers];
AudioMixer4 mixer;
AudioOutputI2S i2s1;
AudioControlSGTL5000 sgtl5000_1;
AudioConnection* patchCords[10];

// ====================== MODES ======================
enum Mode { SCALE_MODE, CHORD_MODE, DRUM_MODE, SYNTH_MODE };
Mode currentMode = SCALE_MODE;
const char* modeNames[] = {"SCALE", "CHORD", "DRUM", "SYNTH"};

// ====================== KEYS ======================
const char* rootNotes[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
int currentKey = 0;
const int keyPattern[] = {0, 2, 4, 5, 7, 9, 11, 12};

// ====================== INPUTS ======================
Encoder enc(32, 31);
Encoder enc2(29, 28);
long lastEncPos = 0;
const int numButtons = 8;
long lastEnc2Pos = 0;
const uint8_t buttonPins[] = {33, 34, 35, 36, 37, 38, 39, 40};
Bounce buttons[numButtons] = {
  {buttonPins[0], 5}, {buttonPins[1], 5}, {buttonPins[2], 5}, {buttonPins[3], 5},
  {buttonPins[4], 5}, {buttonPins[5], 5}, {buttonPins[6], 5}, {buttonPins[7], 5}
};

float volume = 0.7;
int nextVoice = 0;



// ====================== AUDIO PLAYBACK ======================
void playSample(int index) {
  if (index < 0 || index >= 8) return;
  
  // Play the sample from the static buffer using the next available voice (round-robin)
  players[nextVoice].play(sampleBuffer[index]);
  nextVoice = (nextVoice + 1) % numPlayers;
  
  Serial.printf("Triggered button %d (Slot %d)\n", index, index);
}
// ====================== SCALE LOADING FOCUSED ======================

// 1. Load an individual raw file directly into a specific slot in our static buffer
void loadSampleStatic(int index, const char* filename) {
  File f = SD.open(filename);
  if (f) {
    uint32_t bytes = f.size();
    
    // Teensy AudioPlayMemory header requirement:
    // Format identifier (0x81) shifted left + length of sample in 16-bit words (bytes / 2)
    sampleBuffer[index][0] = (0x81UL << 24) | (bytes / 2);
    
    // Read raw PCM data directly into the array, skipping the first word (header)
    f.read((uint8_t*)&sampleBuffer[index][1], bytes);
    f.close();
    
    Serial.printf("Loaded: %s -> Slot %d\n", filename, index);
  } else {
    Serial.printf("Missing file: %s\n", filename);
  }
}

// 2. Loop through the scale pattern, find the correct notes, and load them
void loadScaleForKey(int key) {
  Serial.printf("\n[SCALE] Loading root note: %s\n", rootNotes[key]);
  
  // We loop 8 times to match your 8 buttons
  for (int i = 0; i < 8; i++) {
    // Calculate the actual note index (0-11) based on the root key and scale interval
    int noteIdx = (key + keyPattern[i]) % 12;
    
    char path[32];
    snprintf(path, sizeof(path), "piano/%s.raw", rootNotes[noteIdx]);
    
    // Load the file into the corresponding buffer slot (0 through 7)
    loadSampleStatic(i, path);
  }
}

void changeVolume() {
  long pos2 = enc.read() / 4;
  if (pos2 != lastEnc2Pos) {
    long diff = pos2 - lastEnc2Pos;
    lastEnc2Pos = pos2;
    
    // Adjust volume by 5% steps per encoder click, clamped between 0.0 and 1.0
    //constraint(0.0,1.0);
    volume += (diff * 0.05f);
    if (volume > 1.0f) volume = 1.0f;
    if (volume < 0.0f) volume = 0.0f;
    
    sgtl5000_1.volume(volume);
    Serial.printf("→ Volume: %.f%%\n", volume * 100.0f);
  }
}

void changeKey(){
  long pos = enc.read() / 4;

  if (pos != lastEncPos) {
    currentKey = (currentKey + (pos - lastEncPos) + 12) % 12;
    lastEncPos = pos;
    
    if (currentMode == SCALE_MODE) loadScaleForKey(currentKey);
    // else if (currentMode == CHORD_MODE) loadChordsForKey(currentKey);
      
    Serial.printf("→ Key: %s\n", rootNotes[currentKey]);
  }
}


void checkButtons(){
    // Check buttons for triggers
  for (int i = 0; i < numButtons; i++) {
    buttons[i].update();
    if (buttons[i].fallingEdge()) {
      if (currentMode == SCALE_MODE) {
        playSample(i);
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  Serial.println("=== GrooveBan Mode System ===");

  AudioMemory(350);
  sgtl5000_1.enable();
  sgtl5000_1.volume(volume);

  for(int i=0; i<4; i++) mixer.gain(i, 0.85);

  for(int i = 0; i < numPlayers; i++) {
    patchCords[i] = new AudioConnection(players[i], 0, mixer, i%4);
  }
  patchCords[5] = new AudioConnection(mixer, 0, i2s1, 0);
  patchCords[6] = new AudioConnection(mixer, 0, i2s1, 1);

  if(SD.begin(BUILTIN_SDCARD)) {
    loadScaleForKey(currentKey);
  }

  Serial.printf("Mode: %s | Key: %s\n", modeNames[currentMode], rootNotes[currentKey]);
}



void loop() {
  if (debugTimer.check()) {
    Serial.print("CPU: "); Serial.print(AudioProcessorUsage());
    Serial.print("% | Mem: "); Serial.println(AudioMemoryUsage());
  }
  checkButtons();
  // Check encoder for key changes
  changeKey();
  changeVolume();


}