#include <TM1637Display.h>
#include <Servo.h>

// TM1637 Pins
#define CLK 2
#define DIO 3

// Servo Pin
#define servoPin 5

// Button Pins
#define hourButtonPin 6
#define minuteButtonPin 7

// TM1637 + Servo Setup
TM1637Display display(CLK, DIO);
Servo sprayServo;

// Initial Time (Set your desired starting time)
int hours = 2;       // 2 PM
int minutes = 30;
bool isPM = true;

unsigned long previousMillis = 0;
unsigned long sprayMillis = 0;
const long clockInterval = 60000;   // 1 minute
const long sprayInterval = 5000;    // 5 seconds

// Debounce variables
unsigned long lastHourButtonPress = 0;
unsigned long lastMinuteButtonPress = 0;
const long debounceDelay = 200; // 200ms debounce

void setup() {
  display.setBrightness(0x0f);
  sprayServo.attach(servoPin);
  sprayServo.write(0); // Initial position

  pinMode(hourButtonPin, INPUT_PULLUP);
  pinMode(minuteButtonPin, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();

  // Check hour button
  if (digitalRead(hourButtonPin) == LOW && (currentMillis - lastHourButtonPress > debounceDelay)) {
    lastHourButtonPress = currentMillis;
    hours++;
    if (hours > 12) {
      hours = 1;
    } else if (hours == 12) {
      isPM = !isPM;
    }
  }

  // Check minute button
  if (digitalRead(minuteButtonPin) == LOW && (currentMillis - lastMinuteButtonPress > debounceDelay)) {
    lastMinuteButtonPress = currentMillis;
    minutes++;
    if (minutes >= 60) {
      minutes = 0;
      hours++;
      if (hours > 12) {
        hours = 1;
      } else if (hours == 12) {
        isPM = !isPM;
      }
    }
  }

  // Clock update every minute
  if (currentMillis - previousMillis >= clockInterval) {
    previousMillis = currentMillis;
    minutes++;
    if (minutes >= 60) {
      minutes = 0;
      hours++;
      if (hours > 12) {
        hours = 1;
      } else if (hours == 12) {
        isPM = !isPM;
      }
    }
  }

  // Display time (12-hour format)
  int displayTime = hours * 100 + minutes;
  display.showNumberDecEx(displayTime, 0b01000000, true); // With colon

  // Spray every 5 seconds
  if (currentMillis - sprayMillis >= sprayInterval) {
    sprayMillis = currentMillis;
    sprayServo.write(50);   // Rotate to 60°
    delay(500);             // Hold position
    sprayServo.write(0);    // Back to 0°
  }
}

