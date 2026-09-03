#include <Arduino.h>

//TESTING PUSH !

//MAKE THIS WORK!!!!!!

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
const int ch4Pin = 41;
const int ch3Pin = 24;


unsigned long ch1Value, ch2Value, ch3Value, ch4Value;

double mapRC(unsigned long pulse) {
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
  
  ch1Value = pulseIn(ch1Pin, HIGH, 25000);
  ch4Value = pulseIn(ch4Pin, HIGH, 25000);
  ch3Value = pulseIn(ch3Pin, HIGH, 25000);
  ch2Value = pulseIn(ch2Pin, HIGH, 25000);
 
  
  //Debugging seeing the rawCH1 & rawCh3

  delay(67);

  //Serial.print("Raw Channel 1: "); Serial.print(ch1Value); Serial.print("           Raw Channel 3: "); Serial.print(ch3Value); Serial.println();


  double xPwm = mapRC(ch1Value); // Horizontal vector (X)
  double yPwm = mapRC(ch3Value); // Vertical vector (Y)




  //Serial.print("xPwm : "); Serial.print(xPwm); Serial.print("           yPwm: "); Serial.print(yPwm); Serial.println();



  motors(xPwm, yPwm);
}

void motors(int xPwm, int yPwm) {
  // Center check
  if (xPwm == 0 && yPwm == 0) {
    analogWrite(pwm1, 0);
    analogWrite(pwm2, 0);
    analogWrite(pwm3, 0);
    analogWrite(pwm4, 0);
  }

  // 1. Angle calculation (atan2 returns angle in radians)
  double theta = atan2(yPwm, xPwm);


  // Convert radians to degrees (-180 to +180)
  double thetaDegrees = theta * (180.0 / PI);

  // 2. Pythagorean theorem: speed = sqrt(x^2 + y^2)
  // hypot(x, y) computes sqrt(x*x + y*y) accurately
  int speed = round(hypot(xPwm, yPwm));
  speed = constrain(speed, 0, 255); // Cap at 255 max PWM


  Serial.print("The Speed is: "); Serial.print(speed); Serial.print(" ");



  // 3. Direction selection using degrees
  // -45 deg, +45 deg, +135 deg, -135 deg
  if(speed == 0){
    Serial.println("AT CENTER NOT SUPPOSED TO MOVE");

  }
  else if (thetaDegrees >= -45.0 && thetaDegrees < 45.0) {
    // RIGHT: SE & NE forward
    Serial.println("direction is: Right");
    digitalWrite(dir1, LOW);
    digitalWrite(dir2, HIGH);
    digitalWrite(dir3, LOW);
    digitalWrite(dir4, HIGH);
  } 
  else if (thetaDegrees >= 45.0 && thetaDegrees < 135.0) {
    // FORWARD / UP: Both Norths forward
    Serial.println("direction is: Forward");
    digitalWrite(dir1, HIGH);
    digitalWrite(dir2, HIGH);
    digitalWrite(dir3, LOW);
    digitalWrite(dir4, LOW);
  } 
  else if (thetaDegrees >= -135.0 && thetaDegrees < -45.0) {
    // BACKWARD / DOWN: Both Souths forward
        Serial.println("direction is: Down");
    digitalWrite(dir1, LOW);
    digitalWrite(dir2, LOW);
    digitalWrite(dir3, HIGH);
    digitalWrite(dir4, HIGH);
  } 
  else{
    // LEFT: Both Wests forward (Everything past 135 or -135)
    Serial.println("direction is: Left");
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