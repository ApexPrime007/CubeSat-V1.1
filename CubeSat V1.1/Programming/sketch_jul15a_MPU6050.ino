#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

unsigned long lastUpdate = 0;
const unsigned long interval = 1000; // 1 second = 1000 ms

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(5, 6); // SDA = GPIO5, SCL = GPIO6 on XIAO ESP32S3
  Wire.setClock(100000);

  Serial.println("Initializing MPU6050...");
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) delay(1000);
  }
  Serial.println("MPU6050 initialized.");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  delay(100);
}

void loop() {
  if (millis() - lastUpdate >= interval) {
    lastUpdate = millis(); // update the timestamp

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    float ax = a.acceleration.x;
    float ay = a.acceleration.y;
    float az = a.acceleration.z;
    float totalAcc = sqrt(ax*ax + ay*ay + az*az);
    Serial.print("Total Acceleration (m/s^2): ");
    Serial.println(totalAcc, 2);


    Serial.print("Accel (m/s^2): ");
    Serial.print(a.acceleration.x, 2); Serial.print(", ");
    Serial.print(a.acceleration.y, 2); Serial.print(", ");
    Serial.println(a.acceleration.z, 2);

    Serial.print("Gyro (rad/s): ");
    Serial.print(g.gyro.x, 2); Serial.print(", ");
    Serial.print(g.gyro.y, 2); Serial.print(", ");
    Serial.println(g.gyro.z, 2);

    Serial.print("Temp (°C): ");
    Serial.println(temp.temperature, 2);

    Serial.println("-----------");
  }
}
