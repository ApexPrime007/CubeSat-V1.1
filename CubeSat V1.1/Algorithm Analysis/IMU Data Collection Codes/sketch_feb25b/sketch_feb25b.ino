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

// ---------- Magnetometer Hard-Iron Offsets ----------
float magOffsetX = 0;
float magOffsetY = 0;
float magOffsetZ = 0;

// ---------- Gyro Bias ----------
float gyroBiasX = 0;
float gyroBiasY = 0;
float gyroBiasZ = 0;

// ---------- Magnetometer LPF ----------
float magX_f = 0;
float magY_f = 0;
float magZ_f = 0;
float magAlpha = 0.2f;

// ---------- Timing ----------
unsigned long lastMicros;

void calibrateGyro() {
  Serial.println("Calibrating Gyro...");
  float sumX = 0, sumY = 0, sumZ = 0;
  const int samples = 2000;

  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, t;
    mpu.getEvent(&a, &g, &t);

    sumX += g.gyro.x;
    sumY += g.gyro.y;
    sumZ += g.gyro.z;

    delay(2);
  }

  gyroBiasX = (sumX / samples) * 180.0 / PI;
  gyroBiasY = (sumY / samples) * 180.0 / PI;
  gyroBiasZ = (sumZ / samples) * 180.0 / PI;

  Serial.println("Gyro Calibration Done");
}

void setup() {

  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(400000);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  compass.init();

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  filter.begin(LOOP_FREQ);
  filter.beta = 0.06f;   // tuned for slow spacecraft motion

  delay(2000);
  calibrateGyro();

  lastMicros = micros();
}

void loop() {

  while ((micros() - lastMicros) < (1000000.0 / LOOP_FREQ));
  lastMicros = micros();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // -------- Accelerometer (g) --------
  float ax = a.acceleration.x / 9.81f;
  float ay = a.acceleration.y / 9.81f;
  float az = a.acceleration.z / 9.81f;

  // -------- Gyro (deg/s) --------
  float gx = (g.gyro.x * 180.0 / PI) - gyroBiasX;
  float gy = (g.gyro.y * 180.0 / PI) - gyroBiasY;
  float gz = (g.gyro.z * 180.0 / PI) - gyroBiasZ;

  // -------- Magnetometer --------
  compass.read();
  float mx = compass.getX() - magOffsetX;
  float my = compass.getY() - magOffsetY;
  float mz = compass.getZ() - magOffsetZ;

  // -------- LPF on Magnetometer --------
  magX_f = magAlpha * mx + (1 - magAlpha) * magX_f;
  magY_f = magAlpha * my + (1 - magAlpha) * magY_f;
  magZ_f = magAlpha * mz + (1 - magAlpha) * magZ_f;

  // -------- Update Madgwick --------
  filter.update(gx, gy, gz, ax, ay, az, magX_f, magY_f, magZ_f);

  // -------- Quaternion Output --------
  float q0 = filter.q0;
  float q1 = filter.q1;
  float q2 = filter.q2;
  float q3 = filter.q3;

  // Normalize for safety
  float norm = sqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 /= norm; q1 /= norm; q2 /= norm; q3 /= norm;

  Serial.print("Q: ");
  Serial.print(q0,6); Serial.print(" ");
  Serial.print(q1,6); Serial.print(" ");
  Serial.print(q2,6); Serial.print(" ");
  Serial.println(q3,6);
}