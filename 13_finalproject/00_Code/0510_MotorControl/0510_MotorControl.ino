#include <AccelStepper.h>


// Rotary encoderPins
const int stepPin = D6;    
const int dirPin = D5;    

// Control Pins
const int switchPin = D7;
int switchVal;

// Sensor Pins
const int aseatPin = D0;
const int dseatPin = D8;
const int moisturePin = D1;


// Rotary Encoder Pins
#define CLK 17
#define DT 19
#define SW 18

// Motor Pins
#define STEP_PIN 25
#define DIR_PIN 26

AccelStepper motor(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// Motor State
bool motorEnabled = false;
int stepsToMove = 100;       // Default number of steps
int lastEncoderState = HIGH;
int lastCLK = HIGH;

// Interrupt-safe encoder position tracker
volatile int encoderPos = 10;

// Button debounce
unsigned long lastButtonTime = 0;
const unsigned long debounceDelay = 200;

void IRAM_ATTR handleEncoder() {
  int currentCLK = digitalRead(CLK);
  int currentDT = digitalRead(DT);

  if (currentCLK != lastCLK && currentCLK == LOW) {
    if (currentDT != currentCLK) {
      encoderPos++;
    } else {
      encoderPos--;
    }
  }
  lastCLK = currentCLK;
}

void IRAM_ATTR handleButtonPress() {
  unsigned long now = millis();
  if (now - lastButtonTime > debounceDelay) {
    motorEnabled = !motorEnabled;
    Serial.print("Motor state toggled: ");
    Serial.println(motorEnabled ? "ON" : "OFF");
    lastButtonTime = now;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(CLK), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SW), handleButtonPress, FALLING);

  motor.setMaxSpeed(1000);
  motor.setAcceleration(500);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  Serial.println("System Ready. Turn knob to set steps. Press to toggle motor.");
}

void loop() {
  stepsToMove = constrain(encoderPos * 10, 10, 2000); // Scale and clamp steps
  Serial.print("Steps set to: ");
  Serial.println(stepsToMove);

  if (motorEnabled) {
    motor.moveTo(motor.currentPosition() + stepsToMove);
    while (motor.distanceToGo() != 0) {
      motor.run();
    }
    delay(500);  // Pause after movement
  }

  delay(1000);  // Refresh rate
}
