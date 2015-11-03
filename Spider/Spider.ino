// Pro Mini 3.3V, the spider supplies 4.5V
//
int pirPin = 2;
int spiderPin = 3;
int ledPin = 13;
int pirState = LOW;

unsigned long spiderStartTime = 0;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(spiderPin, OUTPUT);
  digitalWrite(spiderPin, LOW);
  pinMode(ledPin, OUTPUT);
  Serial.begin(38400);
}

void loop(){
  int val = digitalRead(pirPin);
  unsigned long time = millis();

  if (val == HIGH) {
    digitalWrite(ledPin, HIGH);

    if (pirState == LOW) {
      Serial.println("Motion detected!");
      pirState = HIGH;

      if (time - spiderStartTime > 30000) {
        digitalWrite(spiderPin, HIGH);
        spiderStartTime = time;
        delay(10000);
        digitalWrite(spiderPin, LOW);
      }
    }
  }
  else {
    digitalWrite(ledPin, LOW);

    if (pirState == HIGH) {
      Serial.println("Motion ended!");
      pirState = LOW;
    }
  }

  delay(150);
}

