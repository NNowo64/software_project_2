#include <Servo.h>
#include <math.h>

Servo gateServo;

const int trigPin = 12;
const int echoPin = 13;

long duration;
float distance;

bool carDetected = false;
unsigned long lastDetectionTime = 0;

const float DETECT_THRESHOLD = 15.0;
const float CLEAR_THRESHOLD = 25.0;
const unsigned long CLOSE_DELAY = 3000;

double sigmoid(double x) {
  return 1.0 / (1.0 + exp(-x));
}

void liftGate() {
  for (int i = 0; i <= 90; i += 2) {
    double s = sigmoid((i - 45) / 10.0);
    gateServo.write(int(s * 90));
    delay(10);
  }
}

void lowerGate() {
  for (int i = 90; i >= 0; i -= 2) {
    double s = sigmoid((i - 45) / 10.0);
    gateServo.write(int(s * 90));
    delay(10);
  }
}

void setup() {
  Serial.begin(9600);
  gateServo.attach(10);
  gateServo.write(0);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.println("Barrier Ready");
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 30000);
  distance = duration * 0.034 / 2.0;

  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance < DETECT_THRESHOLD && !carDetected) {
    carDetected = true;
    liftGate();
    lastDetectionTime = millis();
  }

  if (carDetected && distance > CLEAR_THRESHOLD && millis() - lastDetectionTime > CLOSE_DELAY) {
    lowerGate();
    carDetected = false;
  }

  delay(100);
}
