// ====== Motor pins ======
const int enableRightMotor = 13;
const int rightMotorPin1 = 5;
const int rightMotorPin2 = 12;

const int enableLeftMotor = 18;
const int leftMotorPin1 = 23;
const int leftMotorPin2 = 19;

// ====== IR sensor pins ======
const int IR_SENSOR_LEFT = A1;
const int IR_SENSOR_RIGHT = A2;

// ====== Parameters ======
const int BASE_SPEED = 130;
const int THRESHOLD = 500;   // สีขาว < 500, สีเขียว > 500

String lastState = "Forward";
String prevState = "Forward";

void setup() {
  Serial.begin(115200);

  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
}

void loop() {
  int leftVal = analogRead(IR_SENSOR_LEFT);
  int rightVal = analogRead(IR_SENSOR_RIGHT);

  bool leftOnWhite = (leftVal < THRESHOLD);
  bool rightOnWhite = (rightVal < THRESHOLD);

  // ==============================
  // 1️⃣ ทั้งสองอยู่บนพื้นสีเขียว → ขับตรง
  // ==============================
  if (!leftOnWhite && !rightOnWhite) {
    lastState = "Forward";
  }
  // ==============================
  // 2️⃣ sensor ซ้ายเจอสีขาว → เลี้ยวขวา
  // ==============================
  else if (leftOnWhite && !rightOnWhite) {
    lastState = "TurnRight";
  }
  // ==============================
  // 3️⃣ sensor ขวาเจอสีขาว → เลี้ยวซ้าย
  // ==============================
  else if (!leftOnWhite && rightOnWhite) {
    lastState = "TurnLeft";
  }
  // ==============================
  // 4️⃣ ทั้งสองเจอสีขาว → ทำคำสั่งล่าสุด
  // ==============================
  // lastState ไม่เปลี่ยน

  // ==============================
  // หน่วงเวลาเล็กน้อย ถ้า state เปลี่ยน
  // ==============================
  if (lastState != prevState) {
    delay(10); 
    prevState = lastState;
  }

  // ==============================
  // ขับมอเตอร์ตาม state
  // ==============================
  if (lastState == "Forward") driveForward(BASE_SPEED, BASE_SPEED);
  else if (lastState == "TurnLeft") turnLeftSmooth();
  else if (lastState == "TurnRight") turnRightSmooth();

  // Debug
  Serial.print("L: "); Serial.print(leftVal);
  Serial.print("  R: "); Serial.print(rightVal);
  Serial.print("  Last: "); Serial.println(lastState);

  delay(5);
}

// ====== Functions ======
void driveForward(int leftPWM, int rightPWM) {
  analogWrite(enableLeftMotor, leftPWM);
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);

  analogWrite(enableRightMotor, rightPWM);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

void turnLeftSmooth() {
  analogWrite(enableLeftMotor, BASE_SPEED * 0.6);   // ล้อซ้ายช้าลง
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, HIGH);

  analogWrite(enableRightMotor, BASE_SPEED);        // ล้อขวาเดินหน้าเต็ม
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}

void turnRightSmooth() {
  analogWrite(enableLeftMotor, BASE_SPEED);         // ล้อซ้ายเดินหน้าเต็ม
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);

  analogWrite(enableRightMotor, BASE_SPEED * 0.6);  // ล้อขวาช้าลง
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, HIGH);
}
