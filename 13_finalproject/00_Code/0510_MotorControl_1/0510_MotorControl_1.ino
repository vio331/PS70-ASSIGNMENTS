#include <AccelStepper.h>

// Motor Pins
const int stepPin = 22;
const int dirPin = 23;

// Encoder Pins
const int swPin = 18;  // Push button
const int dtPin = 19;  // DT pin
const int clkPin = 21; // CLK pin

// LED Pin
const int ledPin = 5;  // LED indicator for motor enabled/disabled

// Variables for encoder state
int lastClkState;
int currentClkState;
unsigned long lastMovementTime = 0;
const int movementTimeout = 100; // Time in ms before motor stops after encoder stops turning
bool isClockwise = true;         // Direction flag
int stepsPerClick = 50;          // How many steps to move per encoder click - ADJUST THIS for response sensitivity
bool motorEnabled = false;       // Motor enabled flag

// Create stepper instance using the AccelStepper library
AccelStepper stepper(1, stepPin, dirPin);

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
  
  // Initialize the stepper motor
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
  
  // Get initial state of the encoder
  lastClkState = digitalRead(clkPin);
  
  Serial.println("System ready.");
  Serial.println("- Press button to enable/disable motor control");
  Serial.println("- When enabled (LED on), turn the encoder to move the motor");
  Serial.println("- Direction of turning controls motor direction");
}

void loop() {
  // Check for button press to toggle motor enabled/disabled
  if (digitalRead(swPin) == LOW) {
    delay(50); // Simple debounce
    if (digitalRead(swPin) == LOW) {
      // Wait until button is released
      while (digitalRead(swPin) == LOW);
      
      // Toggle motor enabled state
      motorEnabled = !motorEnabled;
      
      // Update LED
      digitalWrite(ledPin, motorEnabled ? HIGH : LOW);
      
      Serial.print("Motor control is now: ");
      Serial.println(motorEnabled ? "ENABLED" : "DISABLED");
      
      delay(300); // Additional debounce delay
    }
  }
  
  // Only process encoder and move motor if motor control is enabled
  if (motorEnabled) {
    // Read rotary encoder for direction control
    currentClkState = digitalRead(clkPin);
    
    // If the state of CLK has changed (encoder was rotated)
    if (currentClkState != lastClkState) {
      // Determine direction based on the state of DT compared to CLK
      isClockwise = (digitalRead(dtPin) != currentClkState);
      
      // Move motor in the appropriate direction
      if (isClockwise) {
        stepper.move(stepsPerClick);  // Move clockwise
        Serial.println("Moving clockwise");
      } else {
        stepper.move(-stepsPerClick); // Move counter-clockwise
        Serial.println("Moving counter-clockwise");
      }
      
      // Update the last movement time
      lastMovementTime = millis();
      
      // Update the lastClkState
      lastClkState = currentClkState;
    }
    
    // Run the motor if we're within the movement timeout
    if (millis() - lastMovementTime < movementTimeout) {
      stepper.run();  // This moves the motor toward the target position
    }
  } else {
    // If motor is disabled, just update the encoder state without moving
    currentClkState = digitalRead(clkPin);
    if (currentClkState != lastClkState) {
      lastClkState = currentClkState;
    }
  }
}