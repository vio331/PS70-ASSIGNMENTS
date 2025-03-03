int switchPin = D6;
int switchVal;
int led1Pin = D7;
int led2Pin = D8;
int led3Pin = D9;
int led4Pin = D10;
int photoPin = D0;
int photoVal;
int led5Pin = D5;

void setup() {
  // Action Assignment. do photoPin is analog, so no need here
  Serial.begin(9600);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(led5Pin, OUTPUT);
}

void loop() {
  switchVal = digitalRead(switchPin);
   //Turn one LED on
  if(switchVal == LOW){
    digitalWrite(led1Pin, HIGH);
    digitalWrite(led2Pin, LOW);
    digitalWrite(led3Pin, LOW);
    digitalWrite(led4Pin, LOW);
  }
  //Turn four LEDs on
  else{
    digitalWrite(led1Pin, LOW);
    digitalWrite(led2Pin, HIGH);
    digitalWrite(led3Pin, HIGH);
    digitalWrite(led4Pin, HIGH);
  }

  //When the lights you want are on, it reads the Photocapacitor
  photoVal = analogRead(photoPin); 
  Serial.println(photoVal);

  //Control for LED5
  if(photoVal > 2000)  {
    digitalWrite(led5Pin, HIGH); // Turn on in lower light
  }
  else{
    digitalWrite(led5Pin, LOW); // Turn off in higher light
  }

  delay (500);// just a little lag to allow for a stable reading.

}
