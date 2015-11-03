#include <Servo.h>

#define PIR_INPUT 2

#define LED_EYES 3
#define LED_MOUTH 5

#define LED_HEART 6

#define SERVO_LEFT 9
#define SERVO_RGHT 10
#define SERVO_LEFT_NORMAL 90
#define SERVO_RGHT_NORMAL 90

#define LED_FADE_AMOUNT 5

int pirState = LOW;
int brightnessEyes = 0;
int brightnessMouth = 0;
int brightnessHeart = 0;

unsigned long startTime = 0;
int servoHalfPeriodCount = 0;

boolean normal = false;

Servo servo_left;
Servo servo_rght;

void setup() {
  pinMode(PIR_INPUT, INPUT);

  pinMode(LED_EYES, OUTPUT);
  pinMode(LED_MOUTH, OUTPUT);
  pinMode(LED_HEART, OUTPUT);

  servo_left.attach(SERVO_LEFT);
  servo_rght.attach(SERVO_RGHT);
  servo_left.write(90);
  servo_rght.write(90);

  Serial.begin(38400);
}

void loop() {
  int val = digitalRead(PIR_INPUT);
  unsigned long t = millis();

  if (val == HIGH) {
    if (pirState == LOW) {
      Serial.println("Motion detected!");
      pirState = HIGH;

      if (t - startTime > 120000) {
        startTime = t;
      }
    }
  }
  else {
    if (pirState == HIGH) {
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }
  
  unsigned long dt = t - startTime;
    
  if (startTime == 0 || dt > 100000) {
    if (!normal) {
      Serial.println("Back to normal");
      normal = true;
    }
    
    servoHalfPeriodCount = 0;
    servo_left.write(SERVO_LEFT_NORMAL);
    servo_rght.write(SERVO_RGHT_NORMAL);
    
    brightnessEyes = 0;
    brightnessMouth = 0;
    brightnessHeart = 0;
  }
  else {
    if (normal) {
      Serial.println("Start action");
      normal = false;
    }
    
    if (dt / 1308 > servoHalfPeriodCount) {
      Serial.println("Start Servo action");
      servoHalfPeriodCount = dt / 1308;
      
      if (servoHalfPeriodCount % 2) {
        servo_left.write(SERVO_LEFT_NORMAL - 50);
        servo_rght.write(SERVO_RGHT_NORMAL + 50);
      }
      else {
        servo_left.write(SERVO_LEFT_NORMAL + 50);
        servo_rght.write(SERVO_RGHT_NORMAL - 50);
      }
    }
    
    if (dt / 40 % 2) {
      brightnessHeart = 200;
    }
    else {
      brightnessHeart = 0;
    }
    
    if (dt / 30 % 2) {
      Serial.println("Start led action");
      brightnessEyes = random(0, 200);
      brightnessMouth = random(0, 200);
    }
  }
  
  analogWrite(LED_EYES, brightnessEyes);
  analogWrite(LED_MOUTH, brightnessMouth);
  analogWrite(LED_HEART, brightnessHeart);

  delay(5);
}

