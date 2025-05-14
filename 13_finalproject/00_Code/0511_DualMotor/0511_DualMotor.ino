#include <AccelStepper.h>

// Motor 1 Pins - CHIP ERROR
const int stepPin1 = 22;
const int dirPin1 = 23;

// Motor 2 Pins IS WORKIN
const int stepPin2 = 2;
const int dirPin2 = 4;

// Encoder Pins
const int swPin = 18;  // Push button
const int dtPin = 19;  // DT pin
const int clkPin = 21; // CLK pin

// LED Pin
const int ledPin = 5;  // LED indicator for motors enabled/disabled

// Variables for encoder state
int lastClkState;
int currentClkState;
unsigned long lastMovementTime = 0;
const int movementTimeout = 100; // Time in ms before motors stop after encoder stops turning
bool isClockwise = true;         // Direction flag
int stepsPerClick = 500;          // How many steps to move per encoder click
bool motorsEnabled = false;      // Motors enabled flag

// Create stepper instances using the AccelStepper library
AccelStepper stepper1(1, stepPin1, dirPin1);
AccelStepper stepper2(1, stepPin2, dirPin2);

void setup() {
  // Set up serial communication for debugging
  Serial.begin(9600);
  
  // Configure encoder pins as inputs
  pinMode(swPin, INPUT_PULLUP);
  pinMode(dtPin, INPUT);
  pinMode(clkPin, INPUT);
  
  // Configure LED pin as output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // Start with LED off
  
  // Initialize the stepper motors
  stepper1.setMaxSpeed(1000);
  stepper1.setAcceleration(500);
  
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(500);
  
  // Get initial state of the encoder
  lastClkState = digitalRead(clkPin);
  
  Serial.println("System ready.");
  Serial.println("- Press button to enable/disable motor control");
  Serial.println("- When enabled (LED on), turn the encoder to move both motors");
  Serial.println("- Direction of turning controls motors direction");
}

void loop() {
  // Check for button press to toggle motors enabled/disabled
  if (digitalRead(swPin) == LOW) {
    delay(50); // Simple debounce
    if (digitalRead(swPin) == LOW) {
      // Wait until button is released
      while (digitalRead(swPin) == LOW);
      
      // Toggle motors enabled state
      motorsEnabled = !motorsEnabled;
      
      // Update LED
      digitalWrite(ledPin, motorsEnabled ? HIGH : LOW);
      
      Serial.print("Motors control is now: ");
      Serial.println(motorsEnabled ? "ENABLED" : "DISABLED");
      
      delay(300); // Additional debounce delay
    }
  }
  
  // Only process encoder and move motors if control is enabled
  if (motorsEnabled) {
    // Read rotary encoder for direction control
    currentClkState = digitalRead(clkPin);
    
    // If the state of CLK has changed (encoder was rotated)
    if (currentClkState != lastClkState) {
      // Determine direction based on the state of DT compared to CLK
      isClockwise = (digitalRead(dtPin) != currentClkState);
      
      // Move motors in the appropriate direction
      if (isClockwise) {
        stepper1.move(stepsPerClick);  // Move motor 1 clockwise
        stepper2.move(stepsPerClick);  // Move motor 2 clockwise
        Serial.println("Moving both motors clockwise");
        Serial.println(stepper1.targetPosition());
      } else {
        stepper1.move(-stepsPerClick); // Move motor 1 counter-clockwise
        stepper2.move(-stepsPerClick); // Move motor 2 counter-clockwise
        Serial.println("Moving both motors counter-clockwise");
        Serial.println(stepper1.targetPosition());
      }
      
      // Update the last movement time
      lastMovementTime = millis();
      
      // Update the lastClkState
      lastClkState = currentClkState;
    }
  } else {
    // If motors are disabled, just update the encoder state without moving
    currentClkState = digitalRead(clkPin);
    if (currentClkState != lastClkState) {
      lastClkState = currentClkState;
    }
  }
  stepper1.run();
  stepper2.run();
}