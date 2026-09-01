#include <Arduino.h>

// Define pin connections //

// SE pair
const int pwm1 = 2;  
const int dir1 = 3; 

//SW pair
const int pwm2 = 5;  
const int dir2 = 6; 

// NE pair
const int pwm3 = 8;  
const int dir3 = 9; 

// NW pair
const int pwm4 = 11;  
const int dir4 = 12; 

// flyskypins
const int ch1Pin = 22; 
const int ch2Pin = 33; 
const int ch3Pin = 24;
const int ch4Pin = 35;


double ch1Value, ch2Value, ch3Value, ch4Value ;         // Variable to store Channel 1 pulse width
          // Variable to store Channel 2 pulse width
double yPwm, xPwm;

const double E = 2.718; // approximation of e


void setup() {
  // Initialize Serial communication at 9600 bits per second
  Serial.begin(115200);

  // Set the receiver pins as inputs
  pinMode(ch1Pin, INPUT);
  pinMode(ch2Pin, INPUT);
  pinMode(ch3Pin, INPUT);
  pinMode(ch4Pin, INPUT);

  pinMode(pwm1, OUTPUT);
  pinMode(dir1, OUTPUT);

  pinMode(pwm2, OUTPUT);
  pinMode(dir2, OUTPUT);

  pinMode(pwm3, OUTPUT);
  pinMode(dir3, OUTPUT);

  pinMode(pwm4, OUTPUT);
  pinMode(dir4, OUTPUT);


  Serial.println("--- FlySky FS-iA6 Test Initialized ---");
}

void loop() {
  // pulseIn measures how long the pin stays HIGH in microseconds
  // Timeout set to 25000 microseconds (25ms) to match standard RC frame rates
  ch1Value = pulseIn(ch1Pin, HIGH, 25000);
  ch2Value = pulseIn(ch2Pin, HIGH, 25000);
  ch3Value = pulseIn(ch3Pin, HIGH, 25000);
  ch4Value = pulseIn(ch4Pin, HIGH, 25000);

  // Print the values cleanly to the Serial Monitor
  ch1Value = (ch1Value - 1500)/20;
  ch3Value = (ch3Value - 1500)/20;

  ch1Value = 255 * tanh(ch1Value/6); // rescale the boundaries using tanh
  ch3Value = 255 * tanh(ch3Value/6); // rescale the boundaries using tanh

  ch1Value = round(ch1Value); // round to make it cleaner
  ch3Value = round(ch3Value); // round to make it cleaner

  yPwm = ch3Value; // vertical channel
  xPwm = ch1Value; // horizontal channel

  motors(xPwm, yPwm);
}

// tanh function based on the identity in terms of euler's number.
double tanh(int ch)
{
  double n = pow(E, ch) - pow(E, ch*-1);
  double d = pow(E, ch) + pow(E, ch*-1);
  double output = n/d;
  return n/d;
}

// returns 0 and positive reals.
int relu(int pwm)
{
  if (pwm > 0)
    return pwm;
  else
    return 0;
}

void motors(int xPwm, int yPwm)
{
  // have four outcomes, dependent on atan2()
  double theta = atan2(yPwm, xPwm); // return an angle
  // 
  if (theta >= -PI/4.0 && theta < PI/4.0) // go right, se and ne go forwards.
  {
    // set directions
    digitalWrite(dir1, LOW); //se 
    digitalWrite(dir2, HIGH); //sw
    digitalWrite(dir3, LOW); //ne
    digitalWrite(dir4, HIGH); //nw

    // set speeds
    analogWrite(pwm1, relu(xPwm));
    analogWrite(pwm2, relu(xPwm));
    analogWrite(pwm3, relu(xPwm));
    analogWrite(pwm4, relu(xPwm));  
  }
  else if (theta >= PI/4.0 && theta < 3.0*PI/4.0) // go up, both norths go forward
  {
    // set directions
    digitalWrite(dir1, HIGH); //se
    digitalWrite(dir2, HIGH); //sw
    digitalWrite(dir3, LOW); //ne
    digitalWrite(dir4, LOW); //nw

    // set speeds
    analogWrite(pwm1, relu(yPwm));
    analogWrite(pwm2, relu(yPwm));
    analogWrite(pwm3, relu(yPwm));
    analogWrite(pwm4, relu(yPwm));  
  }
  else if (theta >= -3.0*PI/4.0 && theta < -PI/4.0) // go down, both souths active
  {
    // set directions
    digitalWrite(dir1, LOW); //se
    digitalWrite(dir2, LOW); //sw
    digitalWrite(dir3, HIGH); //ne
    digitalWrite(dir4, HIGH); //nw

    // set speeds
    analogWrite(pwm1, relu(yPwm));
    analogWrite(pwm2, relu(yPwm));
    analogWrite(pwm3, relu(yPwm));
    analogWrite(pwm4, relu(yPwm));  
  }
  else // last one is going left, so both wests are active
  {
    // set directions
    digitalWrite(dir1, HIGH); //se
    digitalWrite(dir2, LOW); //sw
    digitalWrite(dir3, HIGH); //ne
    digitalWrite(dir4, LOW); //nw

    // set speeds
    analogWrite(pwm1, relu(xPwm));
    analogWrite(pwm2, relu(xPwm));
    analogWrite(pwm3, relu(xPwm));
    analogWrite(pwm4, relu(xPwm));  
  }

}