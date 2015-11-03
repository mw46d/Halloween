unsigned long time_array[8][3];

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(0));
  
  unsigned long time = millis();
  
  for (int i = 0; i < 8; i++) {
    pinMode(2 + i, OUTPUT);
    time_array[i][0] = 1;
    time_array[i][1] = time + random(0, 1000 * 1000);
    time_array[i][2] = 0;
  }
}

// the loop function runs over and over again forever
void loop() {
  unsigned long time = millis();
  
  for (int i = 0; i < 8; i++) {
    int k = time_array[i][0];
    
    if (time > time_array[i][k]) {
      if (k == 1) {
        digitalWrite(i + 2, HIGH);
        k = 2;
      }
      else {
        digitalWrite(i + 2, LOW);
        k = 1;
      }
      
      time_array[i][0] = k;
      time_array[i][k] = time + random(0, 1000 * 1000);
    }
  }
  
  delay(10);
}
