/*
   SLK Mini Sumo Board + Waveshare ESP32-S3 Zero
   Dabble GamePad - DIGITAL MODE

   ESP32 Arduino Core 3.x
   Using 5000Hz PWM

   UP              = FORWARD
   DOWN            = BACKWARD
   SQUARE          = TURN LEFT
   CIRCLE          = TURN RIGHT

   MAX PWM = 61
*/

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// --- MOTOR PINS ---
#define MOTOR_L_PWM  7
#define MOTOR_L_DIR  8

#define MOTOR_R_PWM  5
#define MOTOR_R_DIR  6

// --- PWM LIMIT ---
#define MAX_PWM 255

// --- TURNING PWM ---
#define TURN_PWM 20


void setup() {

  Serial.begin(115200);

  pinMode(MOTOR_L_DIR, OUTPUT);
  pinMode(MOTOR_R_DIR, OUTPUT);

  pinMode(MOTOR_L_PWM, OUTPUT);
  pinMode(MOTOR_R_PWM, OUTPUT);

  // Motors OFF
  analogWriteFrequency(MOTOR_L_PWM, 5000);
  analogWriteFrequency(MOTOR_R_PWM, 5000);

  // Motors OFF initially
  analogWrite(MOTOR_L_PWM, 0);
  analogWrite(MOTOR_R_PWM, 0);

  Dabble.begin("SLK_Sumo_Dabble");
}


void loop() {

  Dabble.processInput();

  bool up     = GamePad.isUpPressed();
  bool down   = GamePad.isDownPressed();
  bool square = GamePad.isSquarePressed();
  bool circle = GamePad.isCirclePressed();


  if (up && square) {

  digitalWrite(MOTOR_L_DIR, HIGH);
  digitalWrite(MOTOR_R_DIR, HIGH);

  // LEFT motor FAST, RIGHT motor SLOW
  analogWrite(MOTOR_L_PWM, MAX_PWM);
  analogWrite(MOTOR_R_PWM, TURN_PWM);
}


// UP + CIRCLE = FORWARD + RIGHT
else if (up && circle) {

  digitalWrite(MOTOR_L_DIR, HIGH);
  digitalWrite(MOTOR_R_DIR, HIGH);

  // LEFT motor SLOW, RIGHT motor FAST
  analogWrite(MOTOR_L_PWM, TURN_PWM);
  analogWrite(MOTOR_R_PWM, MAX_PWM);
}


// DOWN + SQUARE = BACKWARD + LEFT
else if (down && square) {

  digitalWrite(MOTOR_L_DIR, LOW);
  digitalWrite(MOTOR_R_DIR, LOW);

  // LEFT motor FAST, RIGHT motor SLOW
  analogWrite(MOTOR_L_PWM, MAX_PWM);
  analogWrite(MOTOR_R_PWM, TURN_PWM);
}


// DOWN + CIRCLE = BACKWARD + RIGHT
else if (down && circle) {

  digitalWrite(MOTOR_L_DIR, LOW);
  digitalWrite(MOTOR_R_DIR, LOW);

  // LEFT motor SLOW, RIGHT motor FAST
  analogWrite(MOTOR_L_PWM, TURN_PWM);
  analogWrite(MOTOR_R_PWM, MAX_PWM);
}


  // =====================================================
  // UP = FORWARD
  // =====================================================
  else if (up) {

    digitalWrite(MOTOR_L_DIR, HIGH);
    digitalWrite(MOTOR_R_DIR, HIGH);

    analogWrite(MOTOR_L_PWM, MAX_PWM);
    analogWrite(MOTOR_R_PWM, MAX_PWM);
  }


  // =====================================================
  // DOWN = BACKWARD
  // =====================================================
  else if (down) {

    digitalWrite(MOTOR_L_DIR, LOW);
    digitalWrite(MOTOR_R_DIR, LOW);

    analogWrite(MOTOR_L_PWM, MAX_PWM);
    analogWrite(MOTOR_R_PWM, MAX_PWM);
  }


  // =====================================================
  // SQUARE = TURN LEFT
  // CORRECTED: SWAPPED FROM PREVIOUS VERSION
  // =====================================================
  else if (square) {

    digitalWrite(MOTOR_L_DIR, HIGH);
    digitalWrite(MOTOR_R_DIR, LOW);

    analogWrite(MOTOR_L_PWM, MAX_PWM);
    analogWrite(MOTOR_R_PWM, MAX_PWM);
  }


  // =====================================================
  // CIRCLE = TURN RIGHT
  // CORRECTED: SWAPPED FROM PREVIOUS VERSION
  // =====================================================
  else if (circle) {

    digitalWrite(MOTOR_L_DIR, LOW);
    digitalWrite(MOTOR_R_DIR, HIGH);

    analogWrite(MOTOR_L_PWM, MAX_PWM);
    analogWrite(MOTOR_R_PWM, MAX_PWM);
  }


  // =====================================================
  // NO BUTTON = STOP
  // =====================================================
  else {

    analogWrite(MOTOR_L_PWM, 0);
    analogWrite(MOTOR_R_PWM, 0);
  }


  // =====================================================
  // SERIAL MONITOR
  // =====================================================
  Serial.print("UP:");
  Serial.print(up);

  Serial.print(" DOWN:");
  Serial.print(down);

  Serial.print(" SQUARE:");
  Serial.print(square);

  Serial.print(" CIRCLE:");
  Serial.println(circle);
}