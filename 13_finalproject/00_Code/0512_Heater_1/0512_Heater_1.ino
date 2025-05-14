// Pin definitions for rotary encoder and outputs
const int swPin = D5;        // Encoder push button
const int dtPin = D4;        // Encoder DT (direction) pin
const int clkPin = D3;       // Encoder CLK (clock) pin
const int ledPin = D6;       // LED indicator pin
const int mosfetPin = D0;    // MOSFET gate control pin (D0 is analog capable)

// PWM settings for ESP32 - using explicit ESP32 PWM functions
const int pwmChannel = 0;    // PWM channel (0-15 on ESP32)
const int pwmFreq = 1000;    // PWM frequency in Hz (1kHz is good for heating)
const int pwmResolution = 8; // 8-bit resolution (0-255)
int heatLevel = 100;         // Default heat level when turned on (0-255)

// State variables
bool isHeating = false;      // Heating state (on/off)
int lastButtonState = HIGH;  // Last button state (pulled up, so HIGH when not pressed)
int lastClkState;            // Last state of encoder clock pin

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  delay(1000); // Short delay to allow serial to start up
  
  // Configure input pins
  pinMode(swPin, INPUT_PULLUP);  // Button with pull-up resistor
  pinMode(dtPin, INPUT);         // Encoder DT pin
  pinMode(clkPin, INPUT);        // Encoder CLK pin
  
  // Configure output pins
  pinMode(ledPin, OUTPUT);       // LED indicator
  
  // Set up PWM for the MOSFET using ESP32's native functions
  // This is more reliable than analogWrite on ESP32
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(mosfetPin, pwmChannel);
  
  // Initialize outputs
  digitalWrite(ledPin, LOW);     // Start with LED off
  ledcWrite(pwmChannel, 0);      // Start with heating off
  
  // Initialize the encoder's last state
  lastClkState = digitalRead(clkPin);
  
  // Print startup message
  Serial.println("\nHeating Pad Control System v1.0");
  Serial.println("- Button: Toggle heat on/off");
  Serial.println("- Rotate: Adjust temperature when on");
  Serial.println("System ready. Heat is OFF");
}

void loop() {
  // Handle button press to toggle heating on/off
  int buttonState = digitalRead(swPin);
  
  // Detect button press (transition from not pressed to pressed)
  if (buttonState == LOW && lastButtonState == HIGH) {
    delay(50); // Simple debounce delay
    
    // Read the button again to confirm it's still pressed
    if (digitalRead(swPin) == LOW) {
      // Toggle heating state
      isHeating = !isHeating;
      
      // Update the heating output and LED
      if (isHeating) {
        digitalWrite(ledPin, HIGH);         // Turn on LED
        ledcWrite(pwmChannel, heatLevel);   // Apply the current heat level
        Serial.print("Heat is ON. Level: ");
        Serial.print(map(heatLevel, 0, 255, 0, 100)); // Convert to percentage
        Serial.println("%");
      } else {
        digitalWrite(ledPin, LOW);          // Turn off LED
        ledcWrite(pwmChannel, 0);           // Turn off heating
        Serial.println("Heat is OFF");
      }
    }
  }
  lastButtonState = buttonState; // Save button state for next comparison
  
  // Only adjust heat level if heating is on
  if (isHeating) {
    // Read encoder rotation
    int clkState = digitalRead(clkPin);
    
    // If clock state changed (rotation detected)
    if (clkState != lastClkState && clkState == HIGH) {
      // Check direction by reading DT pin
      if (digitalRead(dtPin) != clkState) {
        // Counter-clockwise - decrease heat
        if (heatLevel > 50) { // Minimum heat level
          heatLevel -= 5;
        }
      } else {
        // Clockwise - increase heat 
        if (heatLevel < 255) { // Maximum heat level
          heatLevel += 5;
        }
      }
      
      // Apply the new heat level
      ledcWrite(pwmChannel, heatLevel);
      
      // Show the new heat level
      Serial.print("Heat level adjusted: ");
      Serial.print(map(heatLevel, 0, 255, 0, 100)); // Convert to percentage
      Serial.println("%");
    }
    lastClkState = clkState; // Save clock state for next comparison
  }
  
  // Small delay to prevent button bounce
  delay(5);
}