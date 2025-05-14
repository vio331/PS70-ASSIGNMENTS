// Pin definitions
const int swPin = D5;       // Switch input pin
const int mosfetPin1 = D7;  // MOSFET control pin for heater 1
const int mosfetPin2 = D9;  // MOSFET control pin for heater 2
const int mosfetPin3 = D10; // MOSFET control pin for heater 3
const int ledPin = D8;      // LED indicator pin
const int encoderDT = D6;   // Rotary encoder DT pin
const int encoderCLK = D4;  // Rotary encoder CLK pin

// State variables
bool heaterOn = false;       // Tracks if heaters are on or off
int lastSwitchState = HIGH;  // Last switch state (pulled up, so HIGH when not pressed)
int lastCLKState;            // Last state of encoder CLK pin
int lastDTState;             // Last state of encoder DT pin
int heatLevel = 255;         // Current heat level (0-255, start at maximum)
const int heatStep = 15;     // How much to change heat level per encoder click

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Configure pins
  pinMode(swPin, INPUT_PULLUP);     // decoder switch
  pinMode(mosfetPin1, OUTPUT);      // MOSFET for heater 1
  pinMode(mosfetPin2, OUTPUT);      // MOSFET for heater 2
  pinMode(mosfetPin3, OUTPUT);      // MOSFET for heater 3
  pinMode(ledPin, OUTPUT);          // LED indicator
  pinMode(encoderDT, INPUT_PULLUP); // Encoder DT
  pinMode(encoderCLK, INPUT_PULLUP); // Encoder CLK
  
  // Initialize heaters and LED to OFF
  digitalWrite(mosfetPin1, LOW);
  digitalWrite(mosfetPin2, LOW);
  digitalWrite(mosfetPin3, LOW);
  digitalWrite(ledPin, LOW);
  
  // Initialize encoder states
  lastCLKState = digitalRead(encoderCLK);
  lastDTState = digitalRead(encoderDT);
  
  // Print startup message
  Serial.println("Multiple heating pad control");
  Serial.println("Press switch to toggle heaters ON/OFF");
  Serial.println("Rotate dial to adjust heat level for all pads");
  Serial.println("Heaters are OFF");
}

// Function to set heat level for all three heating pads
void setAllHeaters(int level) {
  analogWrite(mosfetPin1, level);
  analogWrite(mosfetPin2, level);
  analogWrite(mosfetPin3, level);
}

void loop() {
  // Read current switch state
  int switchState = digitalRead(swPin);
  
  // Check for switch press (when it changes from not pressed to pressed)
  if (switchState == LOW && lastSwitchState == HIGH) {
    delay(50);
    
    //prevent accidental click?
    if (digitalRead(swPin) == LOW) {
      // Toggle heater state
      heaterOn = !heaterOn;
      
      // Set heaters based on state
      if (heaterOn) {
        setAllHeaters(heatLevel); // Use current heat level for all heaters
        digitalWrite(ledPin, HIGH); // LED on when heaters are on
      } else {
        setAllHeaters(0); // Turn off all heaters
        digitalWrite(ledPin, LOW); // LED off when heaters are off
      }
      
      // Print status message
      if (heaterOn) {
        Serial.print("All heaters are ON at level: ");
        Serial.println(map(heatLevel, 0, 255, 0, 100)); // Show as percentage
      } else {
        Serial.println("All heaters are OFF");
      }
    }
  }
  lastSwitchState = switchState;
  
  // Read current encoder states
  int currentCLKState = digitalRead(encoderCLK);
  int currentDTState = digitalRead(encoderDT);
  
  // If CLK state changed from HIGH to LOW (encoder click)
  if (currentCLKState != lastCLKState && currentCLKState == LOW) {
    // If DT state is different from CLK, it's one direction
    // If they're the same, it's the other direction
    if (currentDTState != currentCLKState) {
      // Clockwise rotation - increase heat
      if (heatLevel < 255) {
        heatLevel = min(255, heatLevel + heatStep);
        Serial.println("Clockwise - Increasing heat");
      }
    } else {
      // Counter-clockwise rotation - decrease heat
      if (heatLevel > 0) {
        heatLevel = max(0, heatLevel - heatStep);
        Serial.println("Counter-clockwise - Decreasing heat");
      }
    }
    
    // If heaters are on, update heat level immediately for all heaters
    if (heaterOn) {
      setAllHeaters(heatLevel);
    }
    
    // Print current heat level
    Serial.print("Heat level set to: ");
    Serial.print(map(heatLevel, 0, 255, 0, 100)); // Show as percentage
    Serial.println("%");
  }
  
  // Update last states
  lastCLKState = currentCLKState;
  lastDTState = currentDTState;
}