// Encoder Pins with the correct pin assignments
const int swPin = D6;    // Push button on D6
const int dtPin = D5;    // DT pin on D5
const int clkPin = D4;   // CLK pin on D4

// State variables
bool isOn = false;  // Current state
int lastButtonState = HIGH;  // Last button state (using pull-up, so HIGH is not pressed)
unsigned long lastDebounceTime = 0;  
unsigned long debounceDelay = 50;    

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  delay(1000); // Give serial time to initialize
  
  // Set encoder pins as inputs
  pinMode(swPin, INPUT_PULLUP);  // Button with pull-up
  pinMode(dtPin, INPUT);         // DT pin
  pinMode(clkPin, INPUT);        // CLK pin
  
  // Initial message
  Serial.println("Button Test - Press the button to toggle ON/OFF");
  Serial.println("Current state: OFF");
}

void loop() {
  // Read the current button state
  int reading = digitalRead(swPin);
  
  // Check if the button state has changed
  if (reading != lastButtonState) {
    // Reset the debounce timer
    lastDebounceTime = millis();
  }
  
  // If the button state has been stable for the debounce delay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button was just pressed (went from HIGH to LOW)
    if (reading == LOW && lastButtonState == HIGH) {
      // Toggle the state
      isOn = !isOn;
      
      // Print the new state - simple ON/OFF message
      if (isOn) {
        Serial.println("ON");
      } else {
        Serial.println("OFF");
      }
    }
  }
  
  // Save the current button state for next comparison
  lastButtonState = reading;
}