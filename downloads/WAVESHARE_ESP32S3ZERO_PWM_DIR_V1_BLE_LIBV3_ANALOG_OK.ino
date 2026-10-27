/*
   SLK Mini Sumo Board + Waveshare ESP32-S3 Zero
   Controlled via Dabble App (Gamepad Joystick)

   ESP32 Arduino Core 3.x
   Using DEFAULT PWM
*/

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// --- PIN DEFINITIONS ---
#define MOTOR_L_PWM 7
#define MOTOR_L_DIR 8

#define MOTOR_R_PWM 5
#define MOTOR_R_DIR 6

void setup() {

  Serial.begin(115200);

  // Direction pins
  pinMode(MOTOR_L_DIR, OUTPUT);
  pinMode(MOTOR_R_DIR, OUTPUT);

  // PWM pins
  pinMode(MOTOR_L_PWM, OUTPUT);
  pinMode(MOTOR_R_PWM, OUTPUT);

  // Set PWM frequency to 5 kHz
  analogWriteFrequency(MOTOR_L_PWM, 5000);
  analogWriteFrequency(MOTOR_R_PWM, 5000);

  // Motors OFF initially
  analogWrite(MOTOR_L_PWM, 0);
  analogWrite(MOTOR_R_PWM, 0);

  // Initial direction
  digitalWrite(MOTOR_L_DIR, LOW);
  digitalWrite(MOTOR_R_DIR, LOW);

  // Initialize Dabble
  Dabble.begin("SLK_Sumo_Dabble");
}

void loop() {

  Dabble.processInput();

  // Read joystick
  // Range: -7 to +7
  int xVal = GamePad.getXaxisData();
  int yVal = GamePad.getYaxisData();

  xVal = -xVal;
  yVal = -yVal;
  // Deadzone
  if (abs(xVal) <= 1 && abs(yVal) <= 1) {

    analogWrite(MOTOR_L_PWM, 0);
    analogWrite(MOTOR_R_PWM, 0);
  }

  else {

    // Differential drive mixing
    int leftSpeed = (yVal * 30) - (xVal * 15);
    int rightSpeed = (yVal * 30) + (xVal * 15);

    // Limit PWM
    leftSpeed = constrain(leftSpeed, -255, 255);
    rightSpeed = constrain(rightSpeed, -255, 255);

    // -------------------------
    // LEFT MOTOR
    // -------------------------
    if (leftSpeed >= 0) {

      digitalWrite(MOTOR_L_DIR, LOW);
      analogWrite(MOTOR_L_PWM, leftSpeed);

    } else {

      digitalWrite(MOTOR_L_DIR, HIGH);
      analogWrite(MOTOR_L_PWM, abs(leftSpeed));
    }

    // -------------------------
    // RIGHT MOTOR
    // -------------------------
    if (rightSpeed >= 0) {

      digitalWrite(MOTOR_R_DIR, LOW);
      analogWrite(MOTOR_R_PWM, rightSpeed);

    } else {

      digitalWrite(MOTOR_R_DIR, HIGH);
      analogWrite(MOTOR_R_PWM, abs(rightSpeed));
    }

    // Serial Monitor
    Serial.print("X: ");
    Serial.print(xVal);

    Serial.print(" | Y: ");
    Serial.print(yVal);

    Serial.print(" -> L Speed: ");
    Serial.print(leftSpeed);

    Serial.print(" | R Speed: ");
    Serial.println(rightSpeed);
  }
}