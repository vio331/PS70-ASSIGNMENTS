// Pin definitions
const int swPin = D5;     // Switch input pin
const int mosfetPin = D7; // MOSFET control pin for heater
const int ledPin = D8;    // LED indicator pin
const int encoderDT = D6; // Rotary encoder DT pin
const int encoderCLK = D4; // Rotary encoder CLK pin

void setup() {
  // put your setup code here, to run once:
   pinMode(swPin, INPUT_PULLUP);    // decoder switch
  pinMode(mosfetPin, OUTPUT);      // MOSFET for heater
  pinMode(ledPin, OUTPUT);         // LED indicator
  pinMode(encoderDT, INPUT_PULLUP); // Encoder DT
  pinMode(encoderCLK, INPUT_PULLUP); // Encoder CLK
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(digitalRead(encoderCLK)); //thing is rotating
  Serial.print("       ");
  Serial.println(digitalRead(encoderDT)); 
}
