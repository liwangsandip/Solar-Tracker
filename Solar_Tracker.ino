#include <Servo.h>

Servo myServo;

const int leftPin = A0;
const int rightPin = A1;

int angle = 90;
const int stepSize = 2;
const int threshold = 50;

void setup() {
  myServo.attach(9);
  myServo.write(angle);
  Serial.begin(9600);
}

void loop() {
  int leftValue = analogRead(leftPin);
  int rightValue = analogRead(rightPin);

  if (leftValue > rightValue + threshold) {
    angle -= stepSize;
  } else if (rightValue > leftValue + threshold) {
    angle += stepSize;
  }
  // else: roughly balanced, hold position

  if (angle < 0) {
    angle = 0;
  } else if (angle > 180) {
    angle = 180;
  }

  myServo.write(angle);

  Serial.print("Left: ");
  Serial.print(leftValue);
  Serial.print("  Right: ");
  Serial.print(rightValue);
  Serial.print("  Angle: ");
  Serial.println(angle);

  delay(500);
}