#include <Arduino.h>

// ===== Ultrasonic pins =====
#define TRIG_LEFT   27
#define ECHO_LEFT   14
#define TRIG_MID    17
#define ECHO_MID    16
#define TRIG_RIGHT  26
#define ECHO_RIGHT  25

// ===== Motor pins =====
const int enableRightMotor = 13;
const int rightMotorPin1 = 5;
const int rightMotorPin2 = 12;

const int enableLeftMotor = 18;
const int leftMotorPin1 = 23;
const int leftMotorPin2 = 19;

// ===== IR sensor pins =====
const int IR_SENSOR_LEFT = A3;
const int IR_SENSOR_RIGHT = A1;

// ===== Parameters =====
const int BASE_SPEED = 90;   // ความเร็วพื้นฐาน
const int TURN_SPEED = 120;  // ความเร็วตอนเลี้ยว
const int STOP_DIST = 13;    // ระยะกำแพงด้านหน้า (เซนติเมตร)
const int SIDE_CLEAR = 18;   // ระยะที่ถือว่า “โล่ง”
const int THRESHOLD_L = 300; // เดิม 300
const int THRESHOLD_R = 600;

String state = "Forward";
String wallDecision = "";

// ====== Ultrasonic Functions ======
double microsecondsToCentimeters(double microseconds) {
  return microseconds / 29.0 / 2.0;
}

double readUltrasonic(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  return microsecondsToCentimeters(pulseIn(echo, HIGH, 20000)); // 20ms timeout
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_LEFT, OUTPUT);  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_MID, OUTPUT);   pinMode(ECHO_MID, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT); pinMode(ECHO_RIGHT, INPUT);

  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  Serial.println("=== Maze Line Follower Initialized ===");
}

// ====== Main Loop ======
void loop() {
  //Test();
  Main();
}
void Main(){
  
  int leftVal = analogRead(IR_SENSOR_LEFT);
  int rightVal = analogRead(IR_SENSOR_RIGHT);

  bool leftWhite = leftVal > THRESHOLD_L;
  bool rightWhite = rightVal > THRESHOLD_R;

  // ===== Check walls before movement =====
  double distL = readUltrasonic(TRIG_LEFT, ECHO_LEFT);
  double distC = readUltrasonic(TRIG_MID, ECHO_MID);
  double distR = readUltrasonic(TRIG_RIGHT, ECHO_RIGHT);

  // ===== Decision Logic =====
  if (distC < STOP_DIST) { // มีกำแพงข้างหน้า
    stopMotor();
    delay(200);

    if (distL > SIDE_CLEAR && distR > SIDE_CLEAR) {
      // ถ้าทั้งซ้ายและขวาโล่ง เลือกทางที่ไกลกว่า
      //if (distL > distR) wallDecision = "Left";
      //else wallDecision = "Right";
      wallDecision = "Left";
    } 
    else if (distL > SIDE_CLEAR) {
      wallDecision = "Left";
    } 
    else if (distR > SIDE_CLEAR) {
      wallDecision = "Right";
    } 
    else {
      wallDecision = "Stop"; // ทั้งคู่ตัน
    }

    if (wallDecision == "Left") {
      Serial.println("Wall ahead! Turning LEFT...");
      turnLeftSmooth();
      delay(530);  // หมุนไป 1.2 วินาที
      stopMotor(); // หมุนประมาณ 90 องศา
      delay(1200);  // หมุนประมาณ 90 องศา
    } 
    else if (wallDecision == "Right") {
      Serial.println("Wall ahead! Turning RIGHT...");
      turnRightSmooth();
      delay(950);  // หมุนไป 1.2 วินาที
      stopMotor(); // หมุนประมาณ 90 องศา
      delay(1200); 
    } 
    else {
      Serial.println("No way out! STOP.");


      //Uturn();
      NewUturn();

    }
  }

  // ===== Line Following =====
  else if (!leftWhite && !rightWhite) {
    // พื้นเขียวทั้งคู่ → เดินตรง
    forwardSmooth();
  } 
  else if (leftWhite && rightWhite) {
    // เจอขาวทั้งคู่ → ยังตรงต่อไป
    forwardSmooth();
  } 
  else if (leftWhite && !rightWhite) {
    // เจอขาวเฉพาะซ้าย → หมุนขวาเล็กน้อย
    turnRightSmooth();
  } 
  else if (!leftWhite && rightWhite) {
    // เจอขาวเฉพาะขวา → หมุนซ้ายเล็กน้อย
    turnLeftSmooth();
  } 
  else {
    forwardSmooth();
  }

  // ===== Debug Output =====
  Serial.print("L: "); Serial.print(leftVal);
  Serial.print("  R: "); Serial.print(rightVal);
  Serial.print("  Dist: L="); Serial.print(distL);
  Serial.print(" C="); Serial.print(distC);
  Serial.print(" R="); Serial.print(distR);
  Serial.print("  State: "); Serial.println(state);

  delay(50);
}

void Test(){
  //Uturn();  // หมุนไป 1.2 วินาที
  //ToiRang();
  NewUturn();
}

// ====== Movement Functions ======
void forwardSmooth() {
  state = "Forward";
  setMotorSpeed(BASE_SPEED, BASE_SPEED);
}

void turnLeftSmooth() {
  state = "TurnLeft";
  setMotorSpeed(BASE_SPEED * 0.1, TURN_SPEED);
}

void turnRightSmooth() {
  state = "TurnRight";
  setMotorSpeed(TURN_SPEED, BASE_SPEED * 0.1);
}

void stopMotor() {
  setMotorSpeed(0, 0);
}

// ====== Motor Driver ======
void setMotorSpeed(int leftPWM, int rightPWM) {
  leftPWM = constrain(leftPWM, 0, 255);
  rightPWM = constrain(rightPWM, 0, 255);

  // ล้อซ้าย
  analogWrite(enableLeftMotor, leftPWM);
  digitalWrite(leftMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);

  // ล้อขวา
  analogWrite(enableRightMotor, rightPWM);
  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(rightMotorPin2, LOW);
}
// ===== Motor Driver แบบรองรับค่าลบ =====
void setLeftMotor(int speed) {
  if(speed >= 0) {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
    analogWrite(enableLeftMotor, constrain(speed, 0, 255));
  } else {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
    analogWrite(enableLeftMotor, constrain(-speed, 0, 255));
  }
}

void setRightMotor(int speed) {
  if(speed >= 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
    analogWrite(enableRightMotor, constrain(speed, 0, 255));
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
    analogWrite(enableRightMotor, constrain(-speed, 0, 255));
  }
}

// ===== UTurn =====
void Backward() {
  setLeftMotor(-140);    // ล้อซ้ายหมุนถอยหลัง
  setRightMotor(130);  // ล้อขวาหมุนถอยหลัง
}
void ToiRang() {
  setLeftMotor(-140);    // ล้อซ้ายหมุนถอยหลัง
  setRightMotor(-130);  // ล้อขวาหมุนถอยหลัง
}

void Uturn(){
  
  delay(700);
  Backward();
  delay(900);  // หมุนไป 1.2 วินาที
  stopMotor();    // หยุดมอเตอร์
}
void NewUturn(){
  setLeftMotor(-140);
  delay(900);   // หมุนไป 1.2 วินาที
  stopMotor();  
  delay(500);    // ล้อซ้ายหมุนถอยหลัง
  setRightMotor(130);
  delay(540);   // หมุนไป 1.2 วินาที
  stopMotor();  
  delay(500);
}