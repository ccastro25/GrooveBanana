#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Encoder.h>
#include <Bounce.h>

// --- Audio Objects (Merged) ---
// Using 8 players for Voice Stealing logic
const int numPlayers = 8;
AudioPlaySdWav           players[numPlayers];
AudioPlaySdWav           KickSDWay; // Kept as specific players for drums
AudioPlaySdWav           BassSdWav4;
AudioPlaySdWav           HigHatSdWav3;
AudioPlaySdWav           SnareSdWav2;

AudioMixer4              Pianomixer3, Bassmixer4, Pianomixer1, mixer2, mixer5;
AudioOutputI2S           i2s1;

// --- Patch Cords ---
AudioConnection          patchCord1(KickSDWay, 0, Bassmixer4, 0);
AudioConnection          patchCord17(Pianomixer3, 0, mixer5, 3);
AudioConnection          patchCord18(Bassmixer4, 0, mixer5, 1);
AudioConnection          patchCord19(Pianomixer1, 0, mixer5, 2);
AudioConnection          patchCord20(mixer2, 0, mixer5, 0);
AudioConnection          patchCord21(mixer5, 0, i2s1, 0);
AudioConnection          patchCord22(mixer5, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;

// --- Inputs ---
const int numButtons = 8;
const int buttonPins[] = {2, 3, 4, 7, 8, 9, 14, 15};
Bounce buttons[numButtons] = {
  Bounce(buttonPins[0], 8), Bounce(buttonPins[1], 8), Bounce(buttonPins[2], 8),
  Bounce(buttonPins[3], 8), Bounce(buttonPins[4], 8), Bounce(buttonPins[5], 8),
  Bounce(buttonPins[6], 8), Bounce(buttonPins[7], 8)
};

Encoder enc2(5, 6);
long lastEncPos = 0;
float volume = 0.5;

// --- Scale & Key Data ---
int nextVoice = 0;
int currentKeyIndex = 0;
const char* keyNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
int majorScale[] = {0, 2, 4, 5, 7, 9, 11}; 
const char* noteFiles[] = {"C0.wav", "C#0.wav", "D0.wav", "D#0.wav", "E0.wav", "F0.wav", "F#0.wav", "G0.wav", "G#0.wav", "A0.wav", "A#0.wav", "B0.wav"};

void setup() {
  Serial.begin(9600);
  AudioMemory(500);
  sgtl5000_1.enable();
  sgtl5000_1.volume(volume);
  if (!(SD.begin(BUILTIN_SDCARD))) Serial.println("SD Error");
  for(int i = 0; i < numButtons; i++) pinMode(buttonPins[i], INPUT_PULLUP);
}

void loop() {
  handleButtons();
  ChangeKey();
}

// --- Interaction Logic ---
void handleButtons() {
  for (int i = 0; i < numButtons; i++) {
    buttons[i].update();
    if (buttons[i].fallingEdge()) {
      // You can mix direct trigger or chord trigger logic here
      playChord(i); 
    }
  }
}

void ChangeKey() {
  long newPos = enc2.read();
  int delta = (newPos - lastEncPos) / 4; 
  if (delta != 0) {
    currentKeyIndex = (currentKeyIndex + delta) % 12;
    if (currentKeyIndex < 0) currentKeyIndex += 12;
    lastEncPos = newPos;
    Serial.print("New Key: "); Serial.println(keyNames[currentKeyIndex]);
  }
}

// --- Voice & Chord Logic ---
void playVoice(int semitone) {
  int targetNote = (currentKeyIndex + semitone) % 12;
  players[nextVoice].stop();
  players[nextVoice].play(noteFiles[targetNote]);
  nextVoice = (nextVoice + 1) % numPlayers;
}

void playChord(int rootDegree) {
  int d1 = majorScale[rootDegree % 7];
  int d2 = majorScale[(rootDegree + 2) % 7];
  int d3 = majorScale[(rootDegree + 4) % 7];
  playVoice(d1);
  playVoice(d2);
  playVoice(d3);
}

/*



for chords 
void playChord(const char* note1, const char* note2, const char* note3) {
  // Use a simple "Round Robin" or static assignment to your players
  pianoSdWav6.play(note1);
  PianoSdWav9.play(note2);
  PianoSdWav10.play(note3);
}

void triggerInstrument(int index) {
  switch(index) {
    case 0: // C Major Button
      playChord("C.wav", "E.wav", "G.wav"); 
      break;
    case 1: // D Minor Button
      playChord("D.wav", "F.wav", "A.wav"); 
      break;
    // ... etc
  }
}


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