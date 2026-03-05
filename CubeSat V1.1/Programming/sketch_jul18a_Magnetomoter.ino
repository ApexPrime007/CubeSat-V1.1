#include <Wire.h>
#include <QMC5883LCompass.h>

QMC5883LCompass compass;

// Define I2C pins for XIAO ESP32S3
#define SDA_PIN 5
#define SCL_PIN 6

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);  // Initialize I2C with XIAO ESP32S3 pins
  Serial.begin(115200);
  delay(500);

  compass.init();
}

void loop() {
  int x, y, z, azimuth, bearing;
  char direction[3];

  compass.read();

  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  azimuth = compass.getAzimuth();
  if (azimuth < 0) azimuth += 360;  // Optional: keep heading in 0–359°

  bearing = compass.getBearing(azimuth);
  compass.getDirection(direction, azimuth);

  Serial.print("X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y);
  Serial.print(" Z: "); Serial.print(z);
  Serial.print(" Azimuth: "); Serial.print(azimuth);
  Serial.print(" Bearing: "); Serial.print(bearing);
  Serial.print(" Direction: ");
  Serial.print(direction[0]);
  Serial.print(direction[1]);
  Serial.print(direction[2]);
  Serial.println();

  delay(250);
}
