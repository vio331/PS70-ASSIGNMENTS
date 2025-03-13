int signalPin = D6;
int photoPin = D0;
int photoVal;
int switchPin = D7;
int switchVal;

void setup() {
  // Action Assignment. do photoPin is analog, so no need here
  Serial.begin(9600);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(signalPin, OUTPUT);
  pinMode(photoPin, INPUT);
}

void loop() {
  switchVal = digitalRead(switchPin);
   //Turn one LED on
  if(switchVal == LOW){
    digitalWrite(photoPin, HIGH);
    delay(500);
    digitalWrite(signalPin, LOW);
  }

  delay (500);// just a little lag to allow for a stable reading.

}
