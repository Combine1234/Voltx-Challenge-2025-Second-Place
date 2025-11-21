// ====== Motor pins ======
const int enableRightMotor = 13;
const int rightMotorPin1 = 5;
const int rightMotorPin2 = 12;

const int enableLeftMotor = 18;
const int leftMotorPin1 = 23;
const int leftMotorPin2 = 19;

// ====== IR sensor pins ======
const int IR_SENSOR_LEFT = A1;   // A1
const int IR_SENSOR_RIGHT = A2;  // A2

// ====== Parameters ======
const int BASE_SPEED = 100;  // ความเร็วพื้นฐาน
const int THRESHOLD = 270;   // สีขาว < 200, สีเขียว > 200

String state = "Forward";

void setup() {
  Serial.begin(115200);

  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  Serial.println("Start line follower: green surface, white edge (smooth mode)");
}

void loop() {
  int leftVal = analogRead(IR_SENSOR_LEFT);
  int rightVal = analogRead(IR_SENSOR_RIGHT);

  bool leftWhite = leftVal > THRESHOLD;
  bool rightWhite = rightVal > 80;

  // ====== Decision Logic ======
  if (!leftWhite && !rightWhite) {
    // ทั้งคู่ > 200 (พื้นสีเขียว) -> เดินตรง
    forwardSmooth();
  } 
  else if (leftWhite && rightWhite) {
    // ทั้งคู่เจอเส้นขาว -> เดินตรง
    forwardSmooth();
  } 
  else if (leftWhite && !rightWhite) {
    // เจอขาวเฉพาะซ้าย -> เลี้ยวซ้าย
    //turnLeftSmooth();
    turnRightSmooth();
  } 
  else if (!leftWhite && rightWhite) {
    // เจอขาวเฉพาะขวา -> เลี้ยวขวา
    //turnRightSmooth();
    turnLeftSmooth();
  } 
  else {
    // เส้นขาด -> ใช้ state ล่าสุด
    if (state == "Forward") forwardSmooth();
    else if (state == "TurnLeft") turnLeftSmooth();
    else if (state == "TurnRight") turnRightSmooth();
  }

  // Debug serial monitor
  Serial.print("L: "); Serial.print(leftVal);
  Serial.print("  R: "); Serial.print(rightVal);
  Serial.print("  State: "); Serial.println(state);

  delay(10);
}

// ====== Movement functions ======
void forwardSmooth() {
  state = "Forward";
  setMotorSpeed(BASE_SPEED, BASE_SPEED);
}

void turnLeftSmooth() {
  state = "TurnLeft";
  setMotorSpeed(BASE_SPEED * 0.05, BASE_SPEED);
}

void turnRightSmooth() {
  state = "TurnRight";
  setMotorSpeed(BASE_SPEED, BASE_SPEED * 0.05);
}

// ====== Set Motor Speed ======
void setMotorSpeed(int leftPWM, int rightPWM) {
  leftPWM = constrain(leftPWM, 0, 255);
  rightPWM = constrain(rightPWM, 0, 255);

  // ล้อซ้ายเดินหน้า
  analogWrite(enableLeftMotor, leftPWM);
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);

  // ล้อขวาเดินหน้า
  analogWrite(enableRightMotor, rightPWM);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}
