// A-Star 32U4
// PIR weiter weg!! Sonst sieht er sich selber als `Bewegung':-(
//
#include <Servo.h> 

#define PIR_PIN     2

#define LED_R       3
#define LED_G       5
#define LED_B       6

#define LED_SKULL  13

#define SERVO_JAW   9
#define JAW_MIN    35
#define JAW_MAX    80
#define SERVO_NECK 10
#define NECK_MIN   40
#define NECK_MID   90
#define NECK_MAX  140

#define AFTER_GLOW  (120UL * 1000)

int pirState = LOW;
int logPirState = pirState;
unsigned long lastTime;

Servo jawServo;
Servo neckServo;

unsigned long startTime, jawStartTime, neckStartTime;
unsigned long lastChange, jawLastChange, neckLastChange;
int jawValue, neckValue;


void setup() {
  pinMode(PIR_PIN, INPUT);
  Serial.begin(57600);
  delay(60000);
  Serial.println("Setup start");

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(LED_SKULL, OUTPUT);
  
  jawServo.attach(SERVO_JAW);
  jawServo.write(JAW_MAX);
  jawValue = JAW_MAX;
  neckServo.attach(SERVO_NECK);
  neckServo.write(NECK_MID);
  neckValue = NECK_MID;
  
  Serial.println("setup done");
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
      jawStartTime = time + random(0, 1500);
      neckStartTime = time + random(0, 2500);
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
    if (time - startTime < 1500) {
      if (time / 30 > lastChange) {
        analogWrite(LED_R, 0);
        analogWrite(LED_G, 0);
        analogWrite(LED_B, 0);
        analogWrite(LED_SKULL, random(0, 150));
        lastChange = time / 30;
      }
    }
    else if (time - startTime < 4000) {
      if (time / 30 > lastChange) {
        analogWrite(LED_SKULL, 0);
        analogWrite(LED_R, random(0, 150));
        analogWrite(LED_G, random(0, 150));
        analogWrite(LED_B, random(0, 150));
        lastChange = time / 30;
      }
    }
    else {
      startTime = time;
    }
    
    if (time > jawStartTime &&  time - jawStartTime < 1500) {
      if (time / 20 > jawLastChange) {
        if (jawValue > JAW_MIN) {
          jawValue--;
        }
        jawLastChange = time / 20;
      }
    }
    else if (time > jawStartTime &&  time - jawStartTime < 3500) {
      if (time / 20 > jawLastChange) {
        if (jawValue < JAW_MAX) {
          jawValue++;
        }
        jawLastChange = time / 20;
      }
    }
    else {
      jawStartTime = time + random(0, 1500);
    }

    if (time > neckStartTime &&  time - neckStartTime < 2000) {
      if (time / 20 > neckLastChange) {
        if (neckValue > NECK_MIN) {
          neckValue--;
        }
        neckLastChange = time / 20;
      }
    }
    else if (time > neckStartTime &&  time - neckStartTime < 4000) {
      if (time / 20 > neckLastChange) {
        if (neckValue < NECK_MAX) {
          neckValue++;
        }
        neckLastChange = time / 20;
      }
    }
    else if (time > neckStartTime &&  time - neckStartTime < 5500) {
      if (time / 20 > neckLastChange) {
        if (neckValue > NECK_MID) {
          neckValue--;
        }
        else if (neckValue < NECK_MID) {
          neckValue++;
        }
        neckLastChange = time / 20;
      }
    }
    else {
      neckStartTime = time + random(0, 3500);
    }
       
    jawServo.write(jawValue);
    neckServo.write(neckValue);
   
    delay(5);
  }
  else {
    analogWrite(LED_SKULL, 0);
    analogWrite(LED_R, 0);
    analogWrite(LED_G, 0);
    analogWrite(LED_B, 0);
    
    jawValue = JAW_MAX;
    neckValue = NECK_MID;
    jawServo.write(jawValue);
    neckServo.write(neckValue);
    
    delay(10);
  }
}

