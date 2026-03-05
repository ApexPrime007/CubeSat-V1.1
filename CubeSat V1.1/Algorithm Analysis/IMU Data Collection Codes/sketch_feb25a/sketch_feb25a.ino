#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <QMC5883LCompass.h>

#define SDA_PIN 5
#define SCL_PIN 6

Adafruit_MPU6050 mpu;
QMC5883LCompass compass;

// -------- Magnetometer Calibration (REPLACE AFTER CALIBRATION) ----------
float magOffsetX = -180;
float magOffsetY = -525;
float magOffsetZ = -600;

// -------- Filtering ----------
float alpha = 0.1;   // Low-pass filter factor
float filteredMagX = 0;
float filteredMagY = 0;

// -------- Complementary filter ----------
float roll = 0;
float pitch = 0;
float yaw = 0;

unsigned long lastTime = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  compass.init();

  lastTime = millis();
}

void loop() {

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  // -------- Read MPU6050 ----------
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  float gx = g.gyro.x;
  float gy = g.gyro.y;
  float gz = g.gyro.z;

  // -------- Compute Roll & Pitch (from accel) ----------
  float accRoll = atan2(ay, az);
  float accPitch = atan2(-ax, sqrt(ay * ay + az * az));

  // -------- Complementary filter for Roll & Pitch ----------
  roll = 0.98 * (roll + gx * dt) + 0.02 * accRoll;
  pitch = 0.98 * (pitch + gy * dt) + 0.02 * accPitch;

  // -------- Read Magnetometer ----------
  compass.read();
  float magX = compass.getX() - magOffsetX;
  float magY = compass.getY() - magOffsetY;
  float magZ = compass.getZ() - magOffsetZ;

  // -------- Low-pass filter mag ----------
  filteredMagX = alpha * magX + (1 - alpha) * filteredMagX;
  filteredMagY = alpha * magY + (1 - alpha) * filteredMagY;

  // -------- Tilt Compensation ----------
  float Xh = filteredMagX * cos(pitch) + magZ * sin(pitch);
  float Yh = filteredMagX * sin(roll) * sin(pitch)
             + filteredMagY * cos(roll)
             - magZ * sin(roll) * cos(pitch);

  float magYaw = atan2(Yh, Xh);

  if (magYaw < 0)
    magYaw += 2 * PI;

  // -------- Complementary filter for Yaw ----------
  yaw = 0.98 * (yaw + gz * dt) + 0.02 * magYaw;

  // -------- Convert to Degrees ----------
  float rollDeg = roll * 180 / PI;
  float pitchDeg = pitch * 180 / PI;
  float yawDeg = yaw * 180 / PI;

  if (yawDeg < 0) yawDeg += 360;

  Serial.print("Roll: "); Serial.print(rollDeg, 2);
  Serial.print(" Pitch: "); Serial.print(pitchDeg, 2);
  Serial.print(" Yaw: "); Serial.println(yawDeg, 2);

  delay(20);
}