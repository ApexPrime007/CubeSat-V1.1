#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <QMC5883LCompass.h>

#define SDA_PIN 5
#define SCL_PIN 6
#define LOOP_FREQ 200.0f

Adafruit_MPU6050 mpu;
QMC5883LCompass compass;

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

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(2000);
  lastMicros = micros();
}

void loop() {

  while ((micros() - lastMicros) < (1000000.0 / LOOP_FREQ));
  lastMicros = micros();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  compass.read();

  // Raw accelerometer (m/s^2)
  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  // Raw gyro (rad/s directly from Adafruit)
  float gx = g.gyro.x;
  float gy = g.gyro.y;
  float gz = g.gyro.z;

  // Raw magnetometer (LSB)
  float mx = compass.getX();
  float my = compass.getY();
  float mz = compass.getZ();

  Serial.print("gx: "); Serial.print(gx,6);
  Serial.print(" gy: "); Serial.print(gy,6);
  Serial.print(" gz: "); Serial.print(gz,6);

  Serial.print(" ax: "); Serial.print(ax,6);
  Serial.print(" ay: "); Serial.print(ay,6);
  Serial.print(" az: "); Serial.print(az,6);

  Serial.print(" mx: "); Serial.print(mx);
  Serial.print(" my: "); Serial.print(my);
  Serial.print(" mz: "); Serial.println(mz);
}