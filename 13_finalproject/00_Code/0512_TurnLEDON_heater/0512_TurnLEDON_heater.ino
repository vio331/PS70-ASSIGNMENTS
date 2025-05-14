//Dial pins
const int swPin = D5;
const int dtPin = D4;
const int clkPin = D3;

//Led Pin
const int ledPin = D6;

//transistor
const int mofsetPin = D0;

// Variables to track button state
int lastButtonState = HIGH;  // Assuming button is pulled up (HIGH when not pressed)
int ledState = LOW;          // LED starts off
bool buttonPressed = false;  // Flag to track button press

void setup() {
  // Configure pin modes
  pinMode(swPin, INPUT_PULLUP);  
  pinMode(dtPin, INPUT);        
  pinMode(clkPin, INPUT);      
  
  pinMode(ledPin, OUTPUT);       
  pinMode(mofsetPin, OUTPUT); 
  
  // Initialize outputs
  digitalWrite(ledPin, ledState);
  digitalWrite(mofsetPin, LOW);
  
  // Optional: Start serial for debugging
  Serial.begin(9600);
  Serial.println("Rotary encoder button to toggle LED");
}

void loop() {
  // Read the current state of the button
  int buttonState = digitalRead(swPin);
  
  // Check if button state changed from HIGH to LOW (button press)
  if (buttonState == LOW && lastButtonState == HIGH) {
    // Button was just pressed
    buttonPressed = true;
  } 
  
  // If button was pressed and now released
  if (buttonState == HIGH && lastButtonState == LOW && buttonPressed) {
    // Toggle LED state
    ledState = !ledState;
    
    // Update LED and transistor
    digitalWrite(ledPin, ledState);
    digitalWrite(mofsetPin, ledState);
    
    // Reset the button pressed flag
    buttonPressed = false;
    
    // Debug print (optional)
    Serial.print("LED state: ");
    Serial.println(ledState ? "ON" : "OFF");
  }
  
  // Save current button state for next comparison
  lastButtonState = buttonState;
  
  // Small delay to prevent bounce
  delay(50);
}