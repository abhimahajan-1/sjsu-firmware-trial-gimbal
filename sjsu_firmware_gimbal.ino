#include <Wire.h>
#include <Servo.h>
#include <math.h>


// Hardware configuration

const uint8_t MPU_ADDR = 0x68;
const uint8_t SERVO_PIN = 9;

// MPU-6050 register addresses
const uint8_t PWR_MGMT_1   = 0x6B;
const uint8_t ACCEL_XOUT_H = 0x3B;

// Servo configuration
const int SERVO_CENTER = 90;
const int SERVO_MIN = 0;
const int SERVO_MAX = 180;


// Global variables

Servo gimbalServo;

float desiredOffsetDeg = 0.0f;

bool reverseServoDirection = false;


// MPU helper functions

void writeMPURegister(uint8_t reg, uint8_t value)
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}


void initializeMPU()
{
  writeMPURegister(PWR_MGMT_1, 0x00);

  delay(100);
}


// Read accelerometer

bool readAccelerometer(int16_t &ax, int16_t &ay, int16_t &az)
{
  Wire.beginTransmission(MPU_ADDR);

  Wire.write(ACCEL_XOUT_H);

  uint8_t status = Wire.endTransmission(false);

  if (status != 0)
  {
    return false;
  }

  uint8_t bytesReceived =
    Wire.requestFrom(MPU_ADDR, (uint8_t)6, (uint8_t)true);

  if (bytesReceived != 6)
  {
    return false;
  }

  ax = ((int16_t)Wire.read() << 8) | Wire.read();
  ay = ((int16_t)Wire.read() << 8) | Wire.read();
  az = ((int16_t)Wire.read() << 8) | Wire.read();

  return true;
}


// Calculate tilt angle

float calculateTiltAngle(int16_t ax, int16_t ay, int16_t az)
{

  float angleRad = atan2((float)ay, (float)az);

  float angleDeg = angleRad * 180.0f / PI;

  return angleDeg;
}



void readSerialOffset()
{
  if (Serial.available() > 0)
  {
    float newOffset = Serial.parseFloat();

    newOffset = constrain(newOffset, -90.0f, 90.0f);

    desiredOffsetDeg = newOffset;

    while (Serial.available() > 0)
    {
      Serial.read();
    }

    Serial.print("New offset set to: ");
    Serial.print(desiredOffsetDeg);
    Serial.println(" degrees");
  }
}


// Calculate servo position

int calculateServoCommand(float measuredAngle)
{
  float servoAngle;

  if (!reverseServoDirection)
  {
    servoAngle =
      SERVO_CENTER
      - measuredAngle
      + desiredOffsetDeg;
  }
  else
  {
    servoAngle =
      SERVO_CENTER
      + measuredAngle
      - desiredOffsetDeg;
  }

  servoAngle = constrain(
    servoAngle,
    SERVO_MIN,
    SERVO_MAX
  );

  return (int)servoAngle;
}


// Setup

void setup()
{
  Serial.begin(115200);

  // Start I2C
  Wire.begin();

  // MPU-6050
  initializeMPU();

  // Set up servo
  gimbalServo.attach(SERVO_PIN);

  // Start servo in center position
  gimbalServo.write(SERVO_CENTER);

  delay(500);

  Serial.println();
  Serial.println("=================================");
  Serial.println("SJSU Robotics Single-Axis Gimbal");
  Serial.println("=================================");
  Serial.println();

  Serial.println("Enter an offset angle in degrees.");
  Serial.println("Examples:");
  Serial.println("0");
  Serial.println("45");
  Serial.println("-30");
  Serial.println();
}


// Main program

void loop()
{
  readSerialOffset();

  int16_t accelX;
  int16_t accelY;
  int16_t accelZ;

  bool success =
    readAccelerometer(
      accelX,
      accelY,
      accelZ
    );

  if (!success)
  {
    Serial.println("ERROR: Could not read MPU-6050");

    delay(100);

    return;
  }

  float tiltAngle =
    calculateTiltAngle(
      accelX,
      accelY,
      accelZ
    );

  // This is where I'm checking to see where the servo should move
  int servoCommand =
    calculateServoCommand(
      tiltAngle
    );

  // Move servo
  gimbalServo.write(servoCommand);


  // Serial output

  Serial.print("AX: ");
  Serial.print(accelX);

  Serial.print(" | AY: ");
  Serial.print(accelY);

  Serial.print(" | AZ: ");
  Serial.print(accelZ);

  Serial.print(" | Tilt: ");
  Serial.print(tiltAngle, 2);

  Serial.print(" deg");

  Serial.print(" | Offset: ");
  Serial.print(desiredOffsetDeg, 2);

  Serial.print(" deg");

  Serial.print(" | Servo: ");
  Serial.print(servoCommand);

  Serial.println(" deg");

  delay(20);
}