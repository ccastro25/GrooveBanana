#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Encoder.h>
#include <Bounce.h>
#include <Metro.h>

Metro debugTimer = Metro(1000);

// --- Audio Objects ---
const int numPlayers = 5; // Updated to 5
AudioPlaySdWav           players[numPlayers];
AudioPlaySdWav           KickSDWay;
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

// Connected 5 players to your input mixers
AudioConnection          p0(players[0], 0, mixer2, 0);
AudioConnection          p1(players[1], 0, mixer2, 1);
AudioConnection          p2(players[2], 0, Pianomixer1, 0);
AudioConnection          p3(players[3], 0, Pianomixer1, 1);
AudioConnection          p4(players[4], 0, Pianomixer3, 0);

AudioControlSGTL5000     sgtl5000_1;

// --- Inputs ---
const int numButtons = 8;
const int buttonPins[] = {33, 34, 35, 36, 37, 38, 39, 40};
Bounce buttons[numButtons] = {
  Bounce(buttonPins[0], 2), Bounce(buttonPins[1], 2), Bounce(buttonPins[2], 2),
  Bounce(buttonPins[3], 2), Bounce(buttonPins[4], 2), Bounce(buttonPins[5], 2),
  Bounce(buttonPins[6], 2), Bounce(buttonPins[7], 2)
};

// --- Timing & Cooldown ---
unsigned long lastChordTime = 0;
const unsigned long SD_COOLDOWN = 100;

Encoder enc2(5, 6);
long lastEncPos = 0;
float volume = 0.5;

// --- Scale & Key Data ---
int nextVoice = 0;
int currentKeyIndex = 0;
const char* keyNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
int majorScale[] = {0, 2, 4, 5, 7, 9, 11}; 
const char* noteFiles[] = {"C.wav", "C#.wav", "D.wav", "D#.wav", "E.wav", "F.wav", "F#.wav", "G.wav", "G#.wav", "A.wav", "A#.wav", "B.wav"};

void setup() {
  Serial.begin(9600);
  AudioMemory(500);
  sgtl5000_1.enable();
  sgtl5000_1.volume(volume);

  Bassmixer4.gain(0, 0.8); 
  mixer5.gain(0, 0.8); mixer5.gain(1, 0.8); mixer5.gain(2, 0.8); mixer5.gain(3, 0.8);
  Pianomixer3.gain(0, 0.8); Pianomixer1.gain(0, 0.8); mixer2.gain(0, 0.8);
  
  if (!(SD.begin(BUILTIN_SDCARD))) Serial.println("SD Error");
  for(int i = 0; i < numButtons; i++) pinMode(buttonPins[i], INPUT_PULLUP);
}

void loop() {
  debug();
  handleButtons();
}

void debug(){
  if (debugTimer.check()) {
    Serial.print("Proc: "); Serial.print(AudioProcessorUsage());
    Serial.print("% | Mem: "); Serial.println(AudioMemoryUsage());
  }
}

void handleButtons() {
  for (int i = 0; i < numButtons; i++) {
    buttons[i].update();
    if (buttons[i].fallingEdge()) {
      playChord(i); 
    }
  }
}

void playVoice(int semitone) {
  int targetNote = (currentKeyIndex + semitone) % 12;
  const char* filename = noteFiles[targetNote];

  if (SD.exists(filename)) {
    players[nextVoice].stop();
    players[nextVoice].play(filename);
    nextVoice = (nextVoice + 1) % numPlayers;
  }
}

void playChord(int rootDegree) {
  if (millis() - lastChordTime < SD_COOLDOWN) return;
  
  playVoice(majorScale[rootDegree % 7]);
  playVoice(majorScale[(rootDegree + 2) % 7]);
  playVoice(majorScale[(rootDegree + 4) % 7]);
  
  lastChordTime = millis();
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