#include <Arduino.h>

// Define pin connections
// SE pair
const int pwm1 = 2;   
const int dir1 = 3; 

// SW pair
const int pwm2 = 5;   
const int dir2 = 6; 

// NE pair
const int pwm3 = 8;   
const int dir3 = 9; 

// NW pair
const int pwm4 = 11;  
const int dir4 = 12; 

// FlySky pins
const int ch1Pin = 22; 
const int ch2Pin = 33; 
const int ch3Pin = 24;
const int ch4Pin = 35;

int mapRC(unsigned long pulse) {
  // Clamp raw pulse to standard RC pulse range (1000us - 2000us)
  long clamped = constrain(pulse, 1000, 2000);

  // Map 1000us-2000us to -255 to 255
  int val = map(clamped, 1000, 2000, -255, 255);

  // Deadband to ignore small center stick jitter
  if (abs(val) < 15) return 0;

  return val;
}

void setup() {
  Serial.begin(115200);

  pinMode(ch1Pin, INPUT);
  pinMode(ch2Pin, INPUT);
  pinMode(ch3Pin, INPUT);
  pinMode(ch4Pin, INPUT);

  pinMode(pwm1, OUTPUT);  pinMode(dir1, OUTPUT);
  pinMode(pwm2, OUTPUT);  pinMode(dir2, OUTPUT);
  pinMode(pwm3, OUTPUT);  pinMode(dir3, OUTPUT);
  pinMode(pwm4, OUTPUT);  pinMode(dir4, OUTPUT);

  Serial.println("--- FlySky FS-iA6 Initialized ---");
}

void loop() {
  unsigned long rawCh1 = pulseIn(ch1Pin, HIGH, 25000);
  unsigned long rawCh3 = pulseIn(ch3Pin, HIGH, 25000);

  int xPwm = mapRC(rawCh1); // Horizontal vector (X)
  int yPwm = mapRC(rawCh3); // Vertical vector (Y)

  motors(xPwm, yPwm);
}

void motors(int xPwm, int yPwm) {
  // Center check
  if (xPwm == 0 && yPwm == 0) {
    analogWrite(pwm1, 0);
    analogWrite(pwm2, 0);
    analogWrite(pwm3, 0);
    analogWrite(pwm4, 0);
    return;
  }

  // 1. Angle calculation (atan2 returns angle in radians: -3.1416 to +3.1416)
  double theta = atan2(yPwm, xPwm);

  // 2. Pythagorean theorem: speed = sqrt(x^2 + y^2)
  // hypot(x, y) computes sqrt(x*x + y*y) accurately
  int speed = round(hypot(xPwm, yPwm));
  speed = constrain(speed, 0, 255); // Cap at 255 max PWM

  // 3. Direction selection using explicit radian values
  // -0.7854 rad = -45 deg,  0.7854 rad = +45 deg,  2.3562 rad = +135 deg
  if (theta >= -0.7854 && theta < 0.7854) {
    // RIGHT: SE & NE forward
    digitalWrite(dir1, LOW);
    digitalWrite(dir2, HIGH);
    digitalWrite(dir3, LOW);
    digitalWrite(dir4, HIGH);
  } 
  else if (theta >= 0.7854 && theta < 2.3562) {
    // FORWARD / UP: Both Norths forward
    digitalWrite(dir1, HIGH);
    digitalWrite(dir2, HIGH);
    digitalWrite(dir3, LOW);
    digitalWrite(dir4, LOW);
  } 
  else if (theta >= -2.3562 && theta < -0.7854) {
    // BACKWARD / DOWN: Both Souths forward
    digitalWrite(dir1, LOW);
    digitalWrite(dir2, LOW);
    digitalWrite(dir3, HIGH);
    digitalWrite(dir4, HIGH);
  } 
  else {
    // LEFT: Both Wests forward
    digitalWrite(dir1, HIGH);
    digitalWrite(dir2, LOW);
    digitalWrite(dir3, HIGH);
    digitalWrite(dir4, LOW);
  }

  // Write resultant magnitude as a speed to all wheels 
  analogWrite(pwm1, speed);
  analogWrite(pwm2, speed);
  analogWrite(pwm3, speed);
  analogWrite(pwm4, speed);
}