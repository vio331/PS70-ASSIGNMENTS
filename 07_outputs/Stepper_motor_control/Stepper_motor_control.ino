#include <AccelStepper.h>

// MOTOR PINS
const int stepPin = D6;    
const int dirPin = D5;    

// SWITCH PIN
const int switchPin = D7;

// SENSOR PINS
const int analog1_pin = D0;
const int seat_pin = D8;
const int seat_moisture_pin = D1;  // Seat moisture sensor pin

// Sensor Thresholds
const int SEAT_SENSOR_THRESHOLD = 1000;
const int SEAT_MOISTURE_WET_THRESHOLD = 1500;
const int SEAT_MOISTURE_DRY_THRESHOLD = 3500;

// Motor configuration
// Note: Changed to use 1 for driver mode (STEP, DIR)
AccelStepper stepper(1, stepPin, dirPin);  

// Variable to track current direction
bool isForward = true;

// Sensor reading function for seat sensor
long readSeatDiff(int samples = 100) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    digitalWrite(seat_pin, HIGH);
    int read_high = analogRead(analog1_pin);
    delayMicroseconds(100);
    digitalWrite(seat_pin, LOW);
    int read_low = analogRead(analog1_pin);
    int diff = read_high - read_low;
    sum += diff;
    delay(2);
  }
  return sum / samples;
}

// Seat moisture sensor reading function
long readSeatMoisture(int samples = 100) {
  long sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(seat_moisture_pin);
    delay(2);
  }
  return sum / samples;
}

void setup() {
  Serial.begin(9600);
 
  // Motor pin setup
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
 
  // speed control
  stepper.setMaxSpeed(2000);        // max steps per second
  stepper.setAcceleration(500);     // Acceleration - measured as steps/second/second
 
  // Set initial direction (HIGH - CLOCKWISE - WHEN IM LOOKING AT IT)
  digitalWrite(dirPin, HIGH);
 
  // Switch setup
  pinMode(switchPin, INPUT_PULLUP);
 
  // Sensor pin setup
  pinMode(seat_pin, OUTPUT);          // set seat pressure pin as input
  pinMode(seat_moisture_pin, INPUT);  // Set seat moisture pin as input

  // Startup message
  Serial.println("Morning Glory Chair");
}

void loop() {
  // Read switch
  int switchVal = digitalRead(switchPin);
 
  // If switch is HIGH (off), stop the motor
  if (switchVal == HIGH) {
    stepper.stop();
    Serial.println("Motor off - no readings");
    return;
  }
 
  // Read seat sensor
  long seatResult = readSeatDiff();
  
  // Read seat moisture sensor
  long seatMoistureResult = readSeatMoisture();
  
  // Print seat moisture reading
  Serial.print("Seat Moisture Reading: ");
  Serial.println(seatMoistureResult);
 
  // Check seat sensor threshold
  if (seatResult > SEAT_SENSOR_THRESHOLD) {
    // Sensor triggered - someone is sitting
    stepper.stop();
    Serial.println("seat is occupied");
    delay(100);
    return;
  }

  // Determine motor direction based on seat moisture
  if (seatMoistureResult < SEAT_MOISTURE_WET_THRESHOLD) {
    // If currently moving forward, reverse
    if (isForward) {
      stepper.stop();
      stepper.setCurrentPosition(0);
      
      // so when the seat is wet reverse the direction
      digitalWrite(dirPin, LOW);  // Set direction pin to reverse
      stepper.setSpeed(-500);     // Negative speed for reverse
      isForward = false;
      Serial.println("Reversing Direction - Seat Too Wet");
    }
  } else {
    // contingency in case the motor is moving in the other direction
    if (!isForward) {
      stepper.stop();
      stepper.setCurrentPosition(0);
      
      // Explicitly set direction and speed for forward
      digitalWrite(dirPin, HIGH);  // Set direction pin to forward
      stepper.setSpeed(500);       // Positive speed for forward
      isForward = true;
      Serial.println("Forward Direction - Seat Moisture Okay");
    }
  }

  // Always run the stepper
  stepper.runSpeed();
}