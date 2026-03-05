#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <QMC5883LCompass.h>
#include <MadgwickAHRS.h>

#define SDA_PIN 5
#define SCL_PIN 6

#define LOOP_FREQ 200.0f
#define DT (1.0f / LOOP_FREQ)

Adafruit_MPU6050 mpu;
QMC5883LCompass compass;
Madgwick filter;

// ======== GYRO BIAS (RAD/S) — FROM YOUR STATIONARY TEST ========
float gyroBiasX = -0.10695256f;
float gyroBiasY =  0.06276361f;
float gyroBiasZ =  0.00290245f;

// ======== MAG OFFSETS (Set after calibration if needed) ========
float magOffsetX = 0;
float magOffsetY = 0;
float magOffsetZ = 0;

// ======== TIMING ========
unsigned long lastMicros;

void setup() {

  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  compass.init();

  // Stable configuration
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Initialize Madgwick
  filter.begin(LOOP_FREQ);
  // filter.beta = 0.04f;   // Stable for slow spacecraft motion

  delay(2000);

  lastMicros = micros();
}

void loop() {

  // ===== Fixed 200 Hz Loop =====
  while ((micros() - lastMicros) < (1000000.0f / LOOP_FREQ));
  lastMicros = micros();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // ===== Accelerometer (convert to g, normalize later) =====
  float ax = a.acceleration.x / 9.81f;
  float ay = a.acceleration.y / 9.81f;
  float az = a.acceleration.z / 9.81f;

  // Normalize accelerometer
  float accNorm = sqrt(ax*ax + ay*ay + az*az);
  if (accNorm > 0.0001f) {
    ax /= accNorm;
    ay /= accNorm;
    az /= accNorm;
  }

  // ===== Gyro (rad/s from Adafruit) =====
  float gx = g.gyro.x - gyroBiasX;
  float gy = g.gyro.y - gyroBiasY;
  float gz = g.gyro.z - gyroBiasZ;

  // ===== Magnetometer =====
  compass.read();

  float mx = compass.getX() - magOffsetX;
  float my = compass.getY() - magOffsetY;
  float mz = compass.getZ() - magOffsetZ;

  // Normalize magnetometer
  float magNorm = sqrt(mx*mx + my*my + mz*mz);
  if (magNorm > 0.0001f) {
    mx /= magNorm;
    my /= magNorm;
    mz /= magNorm;
  }

  // ===== Madgwick Update =====
  filter.update(gx, gy, gz, ax, ay, az, mx, my, mz);

  // ===== Get Quaternion =====
  // STEP - 1: MadgwickAHRS.cpp Change it to: #define betaDef 0.04f Or: float beta = 0.04f;
  // STEP 2 — Expose Quaternion (Flight Grade Requirement) Inside MadgwickAHRS.h, under public section, add:
  /* void getQuaternion(float* _q0, float* _q1, float* _q2, float* _q3) {
      *_q0 = q0;
      *_q1 = q1;
      *_q2 = q2;
      *_q3 = q3;}
    Put it below:
  float getYawRadians() */ 
  // These are the changes inside the Documents/Arduino/libraries/Madgwick/src/MadgwickAHRS.h and .cpp

  float q0, q1, q2, q3;
  filter.getQuaternion(&q0, &q1, &q2, &q3);

  Serial.print("Q: ");
  Serial.print(q0,6); Serial.print(" ");
  Serial.print(q1,6); Serial.print(" ");
  Serial.print(q2,6); Serial.print(" ");
  Serial.println(q3,6);

  // Safety normalization
  float qNorm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 /= qNorm;
  q1 /= qNorm;
  q2 /= qNorm;
  q3 /= qNorm;

  // ===== Output Quaternion (Flight-Ready Format) =====
  Serial.print("Q: ");
  Serial.print(q0, 6); Serial.print(" ");
  Serial.print(q1, 6); Serial.print(" ");
  Serial.print(q2, 6); Serial.print(" ");
  Serial.println(q3, 6);

  float roll = filter.getRoll();
  float pitch = filter.getPitch();
  float yaw = filter.getYaw();

  Serial.print("R: ");
  Serial.print(roll);
  Serial.print(" P: ");
  Serial.print(pitch);
  Serial.print(" Y: ");
  Serial.println(yaw);

  Serial.print(millis());
  Serial.print(" ");
}