// Use a Mega!

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// These are the pins used for the breakout example
#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     7      // VS1053 chip select pin (output)
#define SHIELD_DCS    6      // VS1053 Data/command select pin (output)

char *soundFiles[] = {
  "sound001.mp3",
  "sound002.mp3",
  "sound003.mp3",
};
int numSoundFiles = 3;
unsigned long startSoundTime;
int soundIndex = 0;

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

#define PIR_PIN     2
#define NUM_PINS    10
#define PIN_ON      0
#define PIN_OFF     1
#define AFTER_GLOW  (120UL * 1000)

unsigned long times[NUM_PINS][2];
int pins[NUM_PINS] = { 23, 25, 27, 29, 31, 33, 35, 37, 39, 41 };

int pirState = LOW;             // we start, assuming no motion detected
int logPirState = pirState;
unsigned long lastTime;

void setup() {
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(pins[i], OUTPUT);
  }

  pinMode(PIR_PIN, INPUT);     // declare sensor as input
  Serial.begin(57600);

  if (! musicPlayer.begin()) {
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
     while (1);
  }
  Serial.println(F("VS1053 found"));

  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) {
    Serial.println(F("DREQ pin is not an interrupt pin"));
  }

  musicPlayer.setVolume(0, 0);

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");

  lastTime = millis();
}

void loop(){
  int val = digitalRead(PIR_PIN);  // read input value
  unsigned long time = millis();

  if (val != logPirState) {
    Serial.print("PIR State("); Serial.print(time); Serial.print(")= "); Serial.println(val);
    logPirState = val;
  }

  if (val == HIGH) {
    if (pirState == LOW) {
      for (int i = 0; i < NUM_PINS; i++) {
        if (i % 2 == 0) {
          times[i][PIN_ON] = time + random(500, 2500);
          times[i][PIN_OFF] = times[i][PIN_ON] + random(1000, 10000);
        }
        else {
          times[i][PIN_OFF] = time + random(1000, 10000);
          times[i][PIN_ON] = times[i][PIN_OFF] + random(500, 2500);
        }

        // Serial.print("Times["); Serial.print(i); Serial.print("] = "); Serial.print(times[i][PIN_ON]); Serial.print(", "); Serial.println(times[i][PIN_OFF]);
      }
      startSoundTime = time + random(5000, 100000);
      soundIndex = (soundIndex + 1) % numSoundFiles;
    }

    lastTime = time;
    pirState = HIGH;
  }
  else {
    if (time > lastTime + AFTER_GLOW) {
      pirState = LOW;
      lastTime = time;
    }
  }

  if (pirState == HIGH) {
    for (int i = 0; i < NUM_PINS; i++) {
      if (times[i][PIN_ON] < time && times[i][PIN_OFF] < time) {
        // Create new times
        if (i % 2 == 0) {
          times[i][PIN_ON] = time + random(500, 2500);
          times[i][PIN_OFF] = times[i][PIN_ON] + random(1000, 10000);
        }
        else {
          times[i][PIN_OFF] = time + random(1000, 10000);
          times[i][PIN_ON] = times[i][PIN_OFF] + random(500, 2500);
        }
        // Serial.print("Times["); Serial.print(i); Serial.print("] = "); Serial.print(times[i][PIN_ON]); Serial.print(", "); Serial.println(times[i][PIN_OFF]);
      }

      if (times[i][PIN_ON] < times[i][PIN_OFF]) {
        if (time < times[i][PIN_ON]) {
          digitalWrite(pins[i], HIGH);
        }
        else if (time < times[i][PIN_OFF]) {
          digitalWrite(pins[i], LOW);
        }
      }
      else {
        if (time < times[i][PIN_OFF]) {
          digitalWrite(pins[i], LOW);
        }
        else if (time < times[i][PIN_ON]) {
          digitalWrite(pins[i], HIGH);
        }
      }
    }

    if (startSoundTime < time) {
      if (musicPlayer.playingMusic) {
        startSoundTime = time + random(5000, 100000);
      }
      else {      
        musicPlayer.startPlayingFile(soundFiles[soundIndex]);

        soundIndex = (soundIndex + 1) % numSoundFiles;
      }
    }
  }
  else {
    for (int i = 0; i < NUM_PINS; i++) {
      digitalWrite(pins[i], LOW);
    }
  }
}

