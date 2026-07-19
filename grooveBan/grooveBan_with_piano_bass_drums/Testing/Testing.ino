#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Encoder.h>
#include <Bounce.h>
#include <Metro.h>

Metro debugTimer = Metro(1000);

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
int currentKey = 0;        // 0 = C


// scale format
const int keyPattern = [0,2,4,5,7,9,11];

// Sample storage
struct Sample {
  const unsigned int* data;
  uint32_t len;
};

Sample samples[12] = {0};

// ====================== INPUTS ======================
Encoder enc(5, 6);
long lastEncPos = 0;

const int numButtons = 8;
const int buttonPins[] = {33, 34, 35, 36, 37, 38, 39, 40};
Bounce buttons[numButtons] = {
  Bounce(buttonPins[0], 5), Bounce(buttonPins[1], 5),
  Bounce(buttonPins[2], 5), Bounce(buttonPins[3], 5),
  Bounce(buttonPins[4], 5), Bounce(buttonPins[5], 5),
  Bounce(buttonPins[6], 5), Bounce(buttonPins[7], 5)
};

float volume = 0.7;
int nextVoice = 0;

// ====================== FUNCTIONS ======================
void freeSamples() {
  for(int i = 0; i < 12; i++) {
    if(samples[i].data) {
      free((void*)samples[i].data);
      samples[i].data = nullptr;
    }
  }
}

void loadScaleForKey(int key) {
  freeSamples();
  Serial.printf("\n[SCALE] Loading notes for %s\n", rootNotes[key]);
  
  for(int i = 0; i < 12; i++) {
    int noteIdx = (key + i) % 12;
    char path[32];
    snprintf(path, sizeof(path), "piano00/%s.raw", rootNotes[noteIdx]);
    
    File f = SD.open(path);
    if(f) {
      uint32_t bytes = f.size();
      uint32_t* buf = (uint32_t*)malloc(bytes + 4);
      if(buf) {
        buf[0] = (0x81UL << 24) | (bytes/2);
        f.read((uint8_t*)buf + 4, bytes);
        f.close();
        samples[i].data = (const unsigned int*)buf;
        samples[i].len = bytes/2;
        Serial.printf("  Loaded: %s\n", rootNotes[noteIdx]);
      }
    }
  }
}

void loadChordsForKey(int key) {
  freeSamples();
  Serial.printf("\n[CHORD] Loading chords for %s\n", rootNotes[key]);
  
  const char* chordTypes[3] = {"major", "minor", "dim"};
  
  for(int i = 0; i < 3; i++) {
    char path[48];
    snprintf(path, sizeof(path), "chords/%s_%s.raw", rootNotes[key], chordTypes[i]);
    
    File f = SD.open(path);
    if(f) {
      uint32_t bytes = f.size();
      uint32_t* buf = (uint32_t*)malloc(bytes + 4);
      if(buf) {
        buf[0] = (0x81UL << 24) | (bytes/2);
        f.read((uint8_t*)buf + 4, bytes);
        f.close();
        samples[i].data = (const unsigned int*)buf;
        samples[i].len = bytes/2;
        Serial.printf("  Loaded: %s_%s\n", rootNotes[key], chordTypes[i]);
      }
    } else {
      Serial.print("Missing: "); Serial.println(path);
    }
  }
}

void playSample(int index) {
  if (index < 0 || index >= 12 || samples[index].data == nullptr) return;
  
  players[nextVoice].play(samples[index].data);
  nextVoice = (nextVoice + 1) % numPlayers;
  Serial.printf("Playing sample %d\n", index);
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

  // Encoder - Key change
  long pos = enc.read() / 4;
  if (pos != lastEncPos) {
    currentKey = (currentKey + (pos - lastEncPos) + 12) % 12;
    lastEncPos = pos;
    
    if (currentMode == SCALE_MODE) loadScaleForKey(currentKey);
    else if (currentMode == CHORD_MODE) loadChordsForKey(currentKey);
    
    Serial.printf("→ Key: %s\n", rootNotes[currentKey]);
  }

  // Buttons
  for(int i = 0; i < numButtons; i++) {
    buttons[i].update();
    if(buttons[i].fallingEdge()) {
      Serial.printf("Button %d in %s mode\n", i, modeNames[currentMode]);
      
      if (currentMode == SCALE_MODE) {
        playSample(i);                    // individual note
      } else if (currentMode == CHORD_MODE) {
        playSample(i % 8);                // pre-made chord
      }
    }
  }
}
/*



--------------------------------------------------------


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