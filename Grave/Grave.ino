// Use a Mega!
// Braucht 6V mehr als AA!!
//
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>
#include <Servo.h>
#include <MsTimer2.h>

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
  "sound004.mp3",
  "sound005.mp3",
  "sound006.mp3",
  "sound007.mp3",
  "sound008.mp3",
  "sound009.mp3",
  "sound010.mp3",
};
int numSoundFiles = 10;
unsigned long startSoundTime;
int soundIndex = 0;

// These are common pins between breakout and shield
#define CARDCS 4     // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer =
  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

#define PIR_PIN     2

#define AFTER_GLOW  (120UL * 1000)

#define HEART_PIN    22
#define SERVO_R_PIN  24
#define SERVO_L_PIN  26

#define EYES_PIN     28

#define DIGGER1_EYES_PIN  30
#define DIGGER2_EYES_PIN  32

#define EYES_STONE1_PIN   44
#define SERVO_STONE1_PIN  45
#define EYES_STONE2_PIN   46
#define SERVO_STONE2_PIN  47

#define NUM_CANDLES 10
unsigned long times[NUM_CANDLES][2];
unsigned long stoneTimes[2][2];

int candles[NUM_CANDLES] = { 23, 25, 27, 29, 31, 33, 35, 37, 39, 41 };
#define PIN_ON      0
#define PIN_OFF     1

Servo servo_r;
Servo servo_l;
Servo servo1;
Servo servo2;

boolean heartValue = false;

#define R_LOW 15
#define R_HIGH 65
#define L_LOW 165
#define L_HIGH 90

int pirState = LOW;             // we start, assuming no motion detected
int logPirState = pirState;
unsigned long lastTime;
unsigned long startTime;
unsigned long digger1StartTime;
unsigned long digger2StartTime;
unsigned long heartStartTime;
unsigned long eyesStartTime;
unsigned long armTimes[2][2];
int pos_r = R_LOW;
int pos_l = L_LOW;

void flash() {
  static boolean output = HIGH;

  digitalWrite(HEART_PIN, output);
  output = !output;
}

void setup() {
  Serial.begin(38400);
  delay(15000);
  
  for (int i = 0; i < NUM_CANDLES; i++) {
    pinMode(candles[i], OUTPUT);
  }

  servo_r.attach(SERVO_R_PIN);
  servo_l.attach(SERVO_L_PIN);
  servo1.attach(SERVO_STONE1_PIN);
  servo2.attach(SERVO_STONE2_PIN);
  servo_r.write(R_LOW);
  servo_l.write(L_LOW);
  servo1.write(15);
  servo2.write(15);

  pinMode(HEART_PIN, OUTPUT);
  pinMode(EYES_PIN, OUTPUT);

  pinMode(DIGGER1_EYES_PIN, OUTPUT);
  pinMode(DIGGER2_EYES_PIN, OUTPUT);

  pinMode(EYES_STONE1_PIN, OUTPUT);
  pinMode(EYES_STONE2_PIN, OUTPUT);
  
  pinMode(PIR_PIN, INPUT);
  
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

  MsTimer2::set(75, flash);
  MsTimer2::stop();
  
  Serial.println("Setup done");
}

void loop() {
  int val = digitalRead(PIR_PIN);  // read input value
  unsigned long time = millis();

  if (val != logPirState) {
    Serial.print("PIR State("); Serial.print(time); Serial.print(")= "); Serial.println(val);
    logPirState = val;
  }

  if (val == HIGH) {
    if (pirState == LOW) {
      Serial.println("LOW -> HIGH");
      
      startTime = time;

      for (int i = 0; i < NUM_CANDLES; i++) {
        times[i][PIN_ON] = time + random(500, 4500);
        times[i][PIN_OFF] = times[i][PIN_ON] + random(1000, 5000);
        // Serial.print("Times["); Serial.print(i); Serial.print("] = "); Serial.print(times[i][PIN_ON]); Serial.print(", "); Serial.println(times[i][PIN_OFF]);
      }
      startSoundTime = time + 1000000; // XXX: Sollte gleich anfangen!
      soundIndex = (soundIndex + 1) % numSoundFiles;

      for (int i = 0; i < 2; i++) {
        stoneTimes[i][PIN_ON] = time + random(1000, 2500);
        stoneTimes[i][PIN_OFF] = stoneTimes[i][PIN_ON] + random(1000, 10000);
      }
      
      digger1StartTime = time + random(7000, 15000);
      digger2StartTime = time + random(7000, 15000);
      
      heartStartTime = time + random(8000, 15000);
      heartValue = false;
      eyesStartTime = heartStartTime + random(1000, 3000);
      armTimes[0][PIN_ON] = eyesStartTime + random(500, 1500);
      armTimes[1][PIN_ON] = armTimes[0][PIN_ON] + random(1500, 3500);
      armTimes[0][PIN_OFF] = armTimes[0][PIN_ON] + random(5000, 100000);
      armTimes[1][PIN_OFF] = armTimes[1][PIN_ON] + random(5000, 100000);
      servo_r.attach(SERVO_R_PIN);
      servo_l.attach(SERVO_L_PIN);
    }

    lastTime = time;
    pirState = HIGH;
  }
  else {
    if (time > lastTime + AFTER_GLOW) {
      Serial.println("HIGH -> LOW");
      pirState = LOW;
      lastTime = time;
    }
  }

  if (pirState == HIGH) {
    // Candles
    for (int i = 0; i < NUM_CANDLES; i++) {
      if (times[i][PIN_ON] < time && time < times[i][PIN_OFF]) {
        digitalWrite(candles[i], HIGH);
      }
      else if (times[i][PIN_OFF] < time) {
        digitalWrite(candles[i], LOW);

        // Create new times
        times[i][PIN_ON] = time + random(500, 4500);
        times[i][PIN_OFF] = times[i][PIN_ON] + random(1000, 5000);
        // Serial.print("Times["); Serial.print(i); Serial.print("] = "); Serial.print(times[i][PIN_ON]); Serial.print(", "); Serial.println(times[i][PIN_OFF]);
      }
    }
    
    // Stones
    for (int i = 0; i < 2; i++) {
      if (stoneTimes[i][PIN_ON] < time && time < stoneTimes[i][PIN_OFF]) {
        if (i == 0) {
          digitalWrite(EYES_STONE1_PIN, HIGH);
          
          if (!servo1.attached()) {
            servo1.attach(SERVO_STONE1_PIN);
          }
          
          if ((time / 200) % 2 == 1) {
            servo1.write(110);
          }
          else {
            servo1.write(15);
          }
        }
        else {
          digitalWrite(EYES_STONE2_PIN, HIGH);
          
          if (!servo2.attached()) {
            servo2.attach(SERVO_STONE2_PIN);
          }
          
          if ((time / 200) % 2 == 1) {
            servo2.write(110);
          }
          else {
            servo2.write(15);
          }
        }
      }
      else {
        if (i == 0) {
          digitalWrite(EYES_STONE1_PIN, LOW);
          servo1.write(15);
        }
        else {
          digitalWrite(EYES_STONE2_PIN, LOW);
          servo2.write(15);
        }

        if (stoneTimes[i][PIN_OFF] + 3000 < time) {
          stoneTimes[i][PIN_ON] = time + random(1000, 2500);
          stoneTimes[i][PIN_OFF] = stoneTimes[i][PIN_ON] + random(1000, 10000);
        }
      }
    }

    // Sound
    if (startSoundTime <= time) {
      if (!musicPlayer.playingMusic) {      
        musicPlayer.startPlayingFile(soundFiles[soundIndex]);
        soundIndex = (soundIndex + 1) % numSoundFiles;
      }
    }
    
    if (digger1StartTime < time) {
      digitalWrite(DIGGER1_EYES_PIN, HIGH);
      // Serial.println("digger 1");
    }

    if (digger2StartTime < time) {
      digitalWrite(DIGGER2_EYES_PIN, HIGH);
      // Serial.println("digger 2");
    }

    if (heartStartTime < time && !heartValue) {
      MsTimer2::start();
      // Serial.println("heart");
      heartValue = true;
    }

    if (eyesStartTime < time) {
      digitalWrite(EYES_PIN, HIGH);
      // Serial.println("eyes");
    }

    if (armTimes[0][PIN_OFF] < armTimes[0][PIN_ON]) {
      armTimes[0][PIN_OFF] = armTimes[0][PIN_ON] + random(1500, 5500);
    }
    
    if (armTimes[0][PIN_ON] < time && time < armTimes[0][PIN_OFF]) {
      if (pos_r < R_HIGH) {
        pos_r++;
      }
      servo_r.write(pos_r);
    }
    else if (armTimes[0][PIN_OFF] < time) {
      if (pos_r > R_LOW) {
        pos_r--;
      }
      else if (armTimes[0][PIN_OFF] + 2000 < time) {
        armTimes[0][PIN_ON] = time + random(1500, 5500);
      }
      servo_r.write(pos_r);
    }
    
    if (armTimes[1][PIN_OFF] < armTimes[1][PIN_ON]) {
      armTimes[1][PIN_OFF] = armTimes[1][PIN_ON] + random(1500, 5500);
    }
    
    if (armTimes[1][PIN_ON] < time && time < armTimes[1][PIN_OFF]) {
      if (pos_l > L_HIGH) {
        pos_l--;
      }
      servo_l.write(pos_l);
    }
    else if (armTimes[1][PIN_OFF] < time) {
      if (pos_l < L_LOW) {
        pos_l++;
      }
      else if (armTimes[1][PIN_OFF] + 2000 < time) {
        armTimes[1][PIN_ON] = time + random(1500, 5500);
      }
      servo_l.write(pos_l);
    }
    
    delay(15);
  }
  else {
    MsTimer2::stop();
    heartValue = false;
    digitalWrite(HEART_PIN, LOW);
    
    if (servo_r.attached()) {
      servo_r.write(R_LOW);
    }
    
    if (servo_l.attached()) {
      servo_l.write(L_LOW);
    }
    
    if (servo1.attached()) {
      servo1.write(15);
    }
    
    if (servo2.attached()) {
      servo2.write(15);
    }
    
    pos_r = R_LOW;
    pos_l = L_LOW;
    digitalWrite(EYES_PIN, LOW);
    
    digitalWrite(DIGGER1_EYES_PIN, LOW);
    digitalWrite(DIGGER2_EYES_PIN, LOW);
    
    for (int i = 0; i < NUM_CANDLES; i++) {
      digitalWrite(candles[i], LOW);
    }

    digitalWrite(EYES_STONE1_PIN, LOW);
    digitalWrite(EYES_STONE2_PIN, LOW);
    
    servo_r.detach();
    servo_l.detach();
    servo1.detach();
    servo2.detach();
    
    delay(2000);
  }
}
