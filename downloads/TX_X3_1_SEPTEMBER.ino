/*
  =========================================================
  SLK MINI SUMO BOARD
  Waveshare ESP32-S3 Zero
  T3 X3 Bluetooth Gamepad
  Bluepad32 Library

  AUTHORIZED CONTROLLER:
  03:26:06:24:E1:CB

  CONTROL:

    LEFT JOYSTICK Y  = THROTTLE
    RIGHT JOYSTICK X = STEERING

  T3 X3 OBSERVATION:

    Joystick centre = 0
    Joystick pressed = +/-512

  Therefore:

    SOFTWARE MOTOR RAMPING is used.

  =========================================================
*/
#include <Bluepad32.h>
// MOTOR PIN SETUP
#define M1_EN 7
#define M1_PH 8
#define M2_EN 5
#define M2_PH 6
// AUTHORIZED CONTROLLER MAC
//03:26:04:12:84:FF
const uint8_t AUTHORIZED_MAC[6] = { 0x03, 0x26, 0x04, 0x12, 0x84, 0xFF };
// BLUEPAD32
ControllerPtr myControllers[BP32_MAX_GAMEPADS];
// JOYSTICK SETTINGS
#define JOYSTICK_MAX 512
#define DEADZONE 50
#define MAX_PWM 255
// MOTOR RAMP SETTINGS
// Higher ACCEL_STEP = faster acceleration
// Lower ACCEL_STEP = smoother/slower acceleration
// Higher DECEL_STEP = faster stopping
// Current test:
// ACCEL = 8
// DECEL = 12
#define ACCEL_STEP 8
#define DECEL_STEP 12
// MOTOR CALIBRATION
// Forward
float LEFT_FWD_CAL = 1.00f;
float RIGHT_FWD_CAL = 1.00f;
// Backward
float LEFT_BWD_CAL = 1.00f;
float RIGHT_BWD_CAL = 1.00f;
// Turning
float LEFT_TURN_CAL = 1.00f;
float RIGHT_TURN_CAL = 1.00f;
// CURRENT MOTOR OUTPUT
// These values are ramped gradually toward the target.
float currentLeftPWM = 0.0f;
float currentRightPWM = 0.0f;
// FUNCTION PROTOTYPES
bool isAuthorizedController(ControllerPtr ctl);
void printControllerMAC(ControllerPtr ctl);
void stopRobot();
void setMotor(int pwmPin, int dirPin, int speed);
int processAxis(int value);
int rampValue(float current, int target);
void processGamepad(ControllerPtr ctl);
void processControllers();
// CHECK AUTHORIZED CONTROLLER
bool isAuthorizedController(ControllerPtr ctl) {

  ControllerProperties properties = ctl->getProperties();
  for (int i = 0; i < 6; i++) {
    if (properties.btaddr[i] != AUTHORIZED_MAC[i]) {
      return false;
    }
  }
  return true;
}
// PRINT CONTROLLER MAC
void printControllerMAC(ControllerPtr ctl) {
  ControllerProperties properties = ctl->getProperties();
  Serial.printf(
    "%02X:%02X:%02X:%02X:%02X:%02X\n",properties.btaddr[0],properties.btaddr[1],properties.btaddr[2],properties.btaddr[3],properties.btaddr[4],properties.btaddr[5]);
}
// CONTROLLER CONNECTED
void onConnectedController(ControllerPtr ctl) {
  Serial.println();
  Serial.println("========================================");
  Serial.println("BLUETOOTH CONTROLLER CONNECTED");
  Serial.println("========================================");
  Serial.printf("Index : %d\n", ctl->index());
  Serial.printf("Model : %s\n", ctl->getModelName().c_str());
  Serial.print("MAC   : ");
  printControllerMAC(ctl);
  // CHECK MAC
  if (
    isAuthorizedController(ctl)) {
    Serial.println(
      "STATUS: AUTHORIZED CONTROLLER");
    // Add authorized controller
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
      if (myControllers[i] == nullptr) {
        myControllers[i] = ctl;
        Serial.printf("Controller accepted, slot=%d\n", i);
        break;
      }
    }
  }
  else {
    Serial.println("STATUS: UNAUTHORIZED CONTROLLER");
    Serial.println("Controller ignored.");
    stopRobot();
  }
  Serial.println("========================================");
}
// CONTROLLER DISCONNECTED
void onDisconnectedController(ControllerPtr ctl) {
  Serial.println();
  Serial.println("CONTROLLER DISCONNECTED");
  Serial.print("MAC: ");
  printControllerMAC(ctl);
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      myControllers[i] = nullptr;
      break;
    }
  }
  // Safety stop
  stopRobot();
}
// JOYSTICK DEADZONE
int processAxis(int value) {
  // -----------------------------------------------------
  // DEADZONE
  // -----------------------------------------------------
  if (
    abs(value) < DEADZONE) {
    return 0;
  }
  // -----------------------------------------------------
  // POSITIVE
  // -----------------------------------------------------
  if (value > 0) {
    value = map(value, DEADZONE, JOYSTICK_MAX, 0, JOYSTICK_MAX);
  }
  // -----------------------------------------------------
  // NEGATIVE
  // -----------------------------------------------------
  else {
    value = map(value, -JOYSTICK_MAX, -DEADZONE, -JOYSTICK_MAX, 0);
    }
  return constrain(value,-JOYSTICK_MAX,JOYSTICK_MAX);
}
// MOTOR RAMP
// Moves current output gradually toward target.
// Example:
// Target = 255
// Current:
// 0
// 8
// 16
// 24
// 32
// ...
// 255
//
// =========================================================
int rampValue(float current, int target) {
  // Joystick released
  // Stop immediately
  if (target == 0) {
    return 0;
  }
  // Accelerate toward target
  if (current < target) {
    current += ACCEL_STEP;
    if (current > target) {
      current = target;
    }
  }
  else if (current > target) {
    current -= ACCEL_STEP;
    if (current < target) {
      current = target;
    }
  }
  return (int)current;
}
// MOTOR CONTROL
void setMotor(int pwmPin,int dirPin,int speed) {
  int pwm =abs(speed);
  pwm =constrain(pwm,0,MAX_PWM);
  // FORWARD
  
  if (speed > 0) {
    digitalWrite(dirPin,HIGH);
    analogWrite(pwmPin,pwm);
  }
  // BACKWARD
  else if (speed < 0) {
    digitalWrite(dirPin,LOW);
    analogWrite(pwmPin,pwm);
  }
  // STOP
  else {
    analogWrite(pwmPin,0);
  }
}
// STOP ROBOT
void stopRobot() {
  currentLeftPWM = 0;
  currentRightPWM = 0;
  setMotor(M1_EN,M1_PH,0);
  setMotor(M2_EN,M2_PH,0);
}
// PROCESS GAMEPAD
void processGamepad(ControllerPtr ctl) {
  // LEFT JOYSTICK Y = THROTTLE
  int rawThrottle =ctl->axisY();
  // RIGHT JOYSTICK X = STEERING
  int rawSteering =ctl->axisRX();
  // PROCESS JOYSTICK
  // Forward = -512
  // Therefore invert Y
  int throttle = -processAxis(rawThrottle);
  // Right joystick:
  // Left  = -512
  // Right = +512
  int steering =-processAxis(rawSteering);
  // CONVERT TO -1 ... +1
  float throttlePercent =(float)throttle / JOYSTICK_MAX;
  float steeringPercent =(float)steering / JOYSTICK_MAX;
  // DIFFERENTIAL MIXING
  float leftPercent =throttlePercent + steeringPercent;
  float rightPercent =throttlePercent - steeringPercent;
  // LIMIT
  leftPercent =constrain(leftPercent,-1.0f,1.0f);
  rightPercent =constrain(rightPercent,-1.0f,1.0f);
  // TARGET PWM
  int targetLeftPWM =(int)(leftPercent * MAX_PWM);
  int targetRightPWM =(int)(rightPercent * MAX_PWM);
  // CALIBRATION
  // FORWARD
  if (
    throttlePercent > 0.05f && fabs(steeringPercent) < 0.05f) {
    targetLeftPWM =(int)(targetLeftPWM * LEFT_FWD_CAL);
    targetRightPWM =(int)(targetRightPWM * RIGHT_FWD_CAL);
  }
  // BACKWARD
  else if (throttlePercent < -0.05f && fabs(steeringPercent) < 0.05f) {
    targetLeftPWM =(int)(targetLeftPWM * LEFT_BWD_CAL);
    targetRightPWM =(int)(targetRightPWM * RIGHT_BWD_CAL);
  }
  // TURN RIGHT
  else if (steeringPercent > 0.05f) {
    targetLeftPWM =(int)(targetLeftPWM * LEFT_TURN_CAL);
    targetRightPWM =(int)(targetRightPWM * RIGHT_TURN_CAL);
  }
  // TURN LEFT
  else if (steeringPercent < -0.05f) {
    targetLeftPWM =(int)(targetLeftPWM * LEFT_TURN_CAL);
    targetRightPWM =(int)(targetRightPWM * RIGHT_TURN_CAL);
  }
  // RAMP MOTOR OUTPUT
  currentLeftPWM =rampValue(currentLeftPWM,targetLeftPWM);
  currentRightPWM =rampValue(currentRightPWM,targetRightPWM);
  // SEND TO MOTORS
  setMotor(M1_EN,M1_PH,(int)currentLeftPWM);
  setMotor(M2_EN,M2_PH,(int)currentRightPWM);
  Serial.printf("RAW T:%4d S:%4d | ""TARGET L:%4d R:%4d | ""OUTPUT L:%4d R:%4d\n",rawThrottle,rawSteering,targetLeftPWM,targetRightPWM,(int)currentLeftPWM,(int)currentRightPWM);
}
// PROCESS CONTROLLERS
void processControllers() {
  for (
    auto ctl :myControllers) {
    if (ctl && ctl->isConnected() && ctl->hasData()) {
      if (ctl->isGamepad()) {
        processGamepad(ctl);
      }
    }
  }
}
// SETUP
void setup() {
  Serial.begin(115200);
  delay( 1000);
  Serial.println();
  Serial.println("========================================");
  Serial.println("       SLK MINI SUMO BOARD");
  Serial.println("       T3 X3 BLUETOOTH");
  Serial.println("       MAC LOCK + MOTOR RAMP");
  Serial.println("========================================");
  // SETTINGS
  Serial.printf("Authorized MAC: "
"%02X:%02X:%02X:%02X:%02X:%02X\n",

    AUTHORIZED_MAC[0],
    AUTHORIZED_MAC[1],
    AUTHORIZED_MAC[2],
    AUTHORIZED_MAC[3],
    AUTHORIZED_MAC[4],
    AUTHORIZED_MAC[5]);

  Serial.printf("Deadzone     : %d\n",DEADZONE);
  Serial.printf("Accel Step   : %d\n",ACCEL_STEP);
  Serial.printf("Decel Step   : %d\n",DECEL_STEP);
  // MOTOR PINS
  pinMode(M1_EN,OUTPUT);
  pinMode(M1_PH,OUTPUT);
  pinMode(M2_EN,OUTPUT);
  pinMode(M2_PH,OUTPUT);
  stopRobot();
  // BLUEPAD32
  Serial.printf("Bluepad32 Firmware: %s\n",BP32.firmwareVersion());
  const uint8_t* addr =BP32.localBdAddress();
  Serial.printf("ESP32 BT Address: ""%02X:%02X:%02X:%02X:%02X:%02X\n",addr[0],addr[1],addr[2],addr[3],addr[4],addr[5]);
  // CALLBACKS
  BP32.setup(&onConnectedController,&onDisconnectedController);
  BP32.enableVirtualDevice(false);
  Serial.println();
  Serial.println("Waiting for authorized T3 X3...");
  Serial.println();
}
// MAIN LOOP
void loop() {
  // UPDATE BLUETOOTH
  bool dataUpdated =BP32.update();
  if (dataUpdated) {
    processControllers();
  }
  // FAILSAFE
  bool authorizedConnected =false;
  for (auto ctl :myControllers) {
    if (ctl && ctl->isConnected()) {
      authorizedConnected =true;
      break;
    }
  }

  if (!authorizedConnected) {
    stopRobot();
  }
  // LOOP TIMING
  delay(10);
}
