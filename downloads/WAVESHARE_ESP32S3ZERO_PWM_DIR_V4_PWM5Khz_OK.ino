//Programming untuk board SLK series ver 4 powerd by Waveshare ESP32 S3 Zero
//Programme by Muhammad Masri
//ARDUINO IDE 2.3.10
//ESP32 LIBRARY 3.3.11
//19 SEPTEMBER 2026

// ================= PIN SETUP =================
#define M1_EN 7 // PWM1
#define M1_PH 8 // Direction1

#define M2_EN 5 // PWM2
#define M2_PH 6 // Direction2

// RC input pins
const int chA = 9;  // Steering
const int chB = 10;  // Throttle

// RC calibration
#define NEUTRAL_SPEED     1500
#define NEUTRAL_STEERING  1500
#define RANGE_SPEED       1000
#define RANGE_STEERING    1000

const float DEADBAND = 0.1f;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(chA, INPUT);
  pinMode(chB, INPUT);

  pinMode(M1_EN, OUTPUT);
  pinMode(M1_PH, OUTPUT);

  pinMode(M2_EN, OUTPUT);
  pinMode(M2_PH, OUTPUT);

  analogWriteFrequency(M1_EN, 5000);
  analogWriteFrequency(M2_EN, 5000);
}

// ================= LOOP =================
void loop() {
  int speedPulse = pulseIn(chB, HIGH, 100000);
  int steeringPulse = pulseIn(chA, HIGH, 100000);

  if (speedPulse == 0 || steeringPulse == 0) {
    setMotor(M1_EN, M1_PH, 0);
    setMotor(M2_EN, M2_PH, 0);
    return;
  }
  // Convert the pulse width to percentage (-1.0 to 1.0).
  float speedPercent = (float)(speedPulse - NEUTRAL_SPEED) / (float)(RANGE_SPEED / 2);
  speedPercent = constrain(speedPercent, -1.0f, 1.0f);
    // Add in dead band.
  if ((speedPercent > -0.1f) && (speedPercent < 0.1f)) {
    speedPercent = 0.0f;
  }
  // Convert the pulse width to percentage (-1.0 to 1.0).
  float steeringPercent = (float)(steeringPulse - NEUTRAL_STEERING) / (float)(RANGE_STEERING / 2);
  steeringPercent = constrain(steeringPercent, -1.0f, 1.0f);
  // Add in dead band.
  if ((steeringPercent > -0.1f) && (steeringPercent < 0.1f)) {
    steeringPercent = 0.0f;
  }

  // Mix speed and steering together for left and right motor speed.
  float leftSpeedPercent  = speedPercent + steeringPercent;
  int leftSpeed  = (int)(leftSpeedPercent * 255.0f);
  leftSpeed = constrain(leftSpeed, -255, 255);

  float rightSpeedPercent = speedPercent - steeringPercent;
  int rightSpeed = (int)(rightSpeedPercent * 255.0f);
  rightSpeed = constrain(rightSpeed, -255, 255);
  //leftSpeedPercent  = constrain(leftSpeedPercent, -1.0f, 1.0f);
  //rightSpeedPercent = constrain(rightSpeedPercent, -1.0f, 1.0f);

  applyCalibration(speedPercent, steeringPercent, leftSpeed, rightSpeed);

  setMotor(M1_EN, M1_PH, leftSpeed);
  setMotor(M2_EN, M2_PH, rightSpeed);

  Serial.print("Throttle: ");
  Serial.print(speedPercent, 2);
  Serial.print(" | Steering: ");
  Serial.print(steeringPercent, 2);
  Serial.print(" | L: ");
  Serial.print(leftSpeed);
  Serial.print(" | R: ");
  Serial.println(rightSpeed);

  delay(10);
}

// ================= MOTOR CONTROL =================
void setMotor(int pwmPin, int dirPin, int speed) {
  int pwm = abs(speed);

  if (speed > 0) {
    digitalWrite(dirPin, HIGH);
    analogWrite(pwmPin, pwm);
  } else if (speed < 0) {
    digitalWrite(dirPin, LOW);
    analogWrite(pwmPin, pwm);
  } else {
    analogWrite(pwmPin, 0);
  }
}

// ================= CALIBRATION =================

void applyCalibration(float speedPercent, float steeringPercent, int &leftSpeed, int &rightSpeed) {
  if (speedPercent > 0.0f && steeringPercent == 0.0f) {
    Serial.println("FWD");
    //CALIBRATION SEMASA ROBOT FORWARD
    //jika robot bergerak senget ke arah kanan semasa throttle sahaja ditekan, 
    //kurangkan nilai pada leftspeed. Maximum value 1.0, 
    //kurangkan nilai contohnya 0.95 dan upload semula coding.
    leftSpeed *= 1.0f;  // Adjust for motor imbalance
    rightSpeed *= 1.0f;
  } else if (speedPercent < 0.0f && steeringPercent == 0.0f) {
    Serial.println("BWD");
    //CALIBRATION SEMASA ROBOT BACKWARD
    leftSpeed *= 1.0f;
    rightSpeed *= 1.0f;
  } else if (steeringPercent > 0.0f) {
    Serial.println("RIGHT");
    //CALIBRATION SEMASA ROBOT TURN RIGHT
    leftSpeed *= 1.0f;
    rightSpeed *= 1.0f;
  } else if (steeringPercent < 0.0f) {
    Serial.println("LEFT");
    //CALIBRATION SEMASA ROBOT TURN LEFT
    leftSpeed *= 1.0f;
    rightSpeed *= 1.0f;
  }
}