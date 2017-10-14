#define keyNumber 4
byte keyStatus[4];
byte keyMapping[] = { 7, 8, 9, 10 };
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
