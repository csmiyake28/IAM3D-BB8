#include <Arduino.h>

//TESTING PUSH !

//MAKE THIS WORK!!!!!!

// Define pin connections
// SE pair
const int SEin1 = 2;   
const int SEin2 = 3; 

// SW pair
const int SWin1 = 5;   
const int SWin2 = 6; 

// NE pair
const int NEin1 = 8;   
const int NEin2 = 9; 

// NW pair
const int NWin1 = 11;  
const int NWin2 = 12; 

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

  pinMode(SEin1, OUTPUT);  pinMode(SEin2, OUTPUT);
  pinMode(SWin1, OUTPUT);  pinMode(SWin2, OUTPUT);
  pinMode(NEin1, OUTPUT);  pinMode(NEin2, OUTPUT);
  pinMode(NWin2, OUTPUT);  pinMode(NWin1, OUTPUT);

  Serial.println("--- FlySky FS-iA6 Initialized ---");
}

void loop() {
  
  ch1Value = pulseIn(ch1Pin, HIGH, 25000);
  ch4Value = pulseIn(ch4Pin, HIGH, 25000);
  ch3Value = pulseIn(ch3Pin, HIGH, 25000);
  ch2Value = pulseIn(ch2Pin, HIGH, 25000);
 
  
  //Debugging seeing the rawCH1 & rawCh3

  delay(50);

  //Serial.print("Raw Channel 1: "); Serial.print(ch1Value); Serial.print("           Raw Channel 3: "); Serial.print(ch3Value); Serial.println();


  double xPwm = mapRC(ch1Value); // Horizontal vector (X)
  double yPwm = mapRC(ch3Value); // Vertical vector (Y)




  //Serial.print("xPwm : "); Serial.print(xPwm); Serial.print("           yPwm: "); Serial.print(yPwm); Serial.println();



  motors(xPwm, yPwm);
}

void motors(int xPwm, int yPwm) {
  

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
  if(speed < 30){
    Serial.println("AT CENTER NOT SUPPOSED TO MOVE");
  }
  else if (thetaDegrees >= -45.0 && thetaDegrees < 45.0) {
    // RIGHT: SE & NE forward
    Serial.println("direction is: EAST");
    analogWrite(NWin1, LOW); digitalWrite(NWin2, speed); // NW motors will spin Backward
    analogWrite(NEin1, speed); digitalWrite(NEin2, LOW); // NE motors will spin forward
    analogWrite(SWin1, LOW);   digitalWrite(SWin2, speed); // SW motors will spin Backward
    analogWrite(SEin1, speed);   digitalWrite(SEin2, LOW); //  SE motors will spin Forward


  } 
  else if (thetaDegrees >= 45.0 && thetaDegrees < 135.0) {
    // FORWARD / UP: Both Norths forward
    Serial.println("direction is: NORTH");
    analogWrite(NWin1, speed); digitalWrite(NWin2, LOW); // NW motors will spin forward
    analogWrite(NEin1, speed); digitalWrite(NEin2, LOW); // NE motors will spin forward
    analogWrite(SWin1, LOW);   digitalWrite(SWin2, speed); // SW motors will spin Backward
    analogWrite(SEin1, LOW);   digitalWrite(SEin2, speed); //  SE motors will spin Backward
    

  } 
  else if (thetaDegrees >= -135.0 && thetaDegrees < -45.0) {
    // BACKWARD / DOWN: Both Souths forward
    Serial.println("direction is: SOUTH");
    analogWrite(NWin1, LOW); digitalWrite(NWin2, speed); // NW motors will spin Backward
    analogWrite(NEin1, LOW); digitalWrite(NEin2, speed); // NE motors will spin Backward
    analogWrite(SWin1, speed);   digitalWrite(SWin2, LOW); // SW motors will spin Forward
    analogWrite(SEin1, speed);   digitalWrite(SEin2, LOW); //  SE motors will spin Forward
  } 
  else{
    // LEFT: Both Wests forward (Everything past 135 or -135)
    Serial.println("direction is: WEST");
    analogWrite(NWin1, speed); digitalWrite(NWin2, LOW); // NW motors will spin Forward
    analogWrite(NEin1, LOW); digitalWrite(NEin2, speed); // NE motors will spin Backward
    analogWrite(SWin1, speed);   digitalWrite(SWin2, LOW); // SW motors will spin Forward
    analogWrite(SEin1, LOW);   digitalWrite(SEin2, speed); //  SE motors will spin Backward
  }
}




/* NEEd to fix: 

  1) Fire up Both Dir are correct add SW needs to be CCLK SE needs to CLK


  2)Fire Right: NW CCLK NE CCLK SE: CLK SW: CLK 

  3) FIRE Down:  NW: CLK NE: CCLK SW: CLK SE: CCLK

  4) Fire Left: NW: CLK NE: CLK SW: CCLK SE: CLK




*/