#define keyNumber 8
byte keyStatus[8];
byte keyMapping[] = { 6, 7, 8, 9, 10, 11, 12, 13 };
byte currentKey, currentStatus;

void setup() {
  Serial.begin(9600);

  for ( int i = 0; i < keyNumber; i++ ){
    keyStatus[i] = 0;
    pinMode( keyMapping[i], OUTPUT );
  }
}

void loop() {
  for ( int i = 0; i < keyNumber; i++ ){
    currentKey = keyMapping[i];
    currentStatus = digitalRead(currentKey);
    
    if ( currentStatus != keyStatus[ i ] ){
      keyStatus[i] = currentStatus;
      Serial.write((currentStatus<<7)|currentKey);
    }
  }

  delay(50);
}
