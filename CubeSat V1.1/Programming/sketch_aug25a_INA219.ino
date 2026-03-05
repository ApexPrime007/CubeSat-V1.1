#include <Wire.h>
#include <Adafruit_INA219.h>

// Create INA219 instance
Adafruit_INA219 ina219;

// Use custom I2C on GPIO 5 (SDA), GPIO 6 (SCL)
#define I2C_SDA 5
#define I2C_SCL 6

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Start I2C with custom pins
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize INA219 with the custom Wire instance
  if (!ina219.begin(&Wire)) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

  Serial.println("INA219 sensor initialized");
}

void loop() {
  float busVoltage_V = ina219.getBusVoltage_V();
  float shuntVoltage_mV = ina219.getShuntVoltage_mV();
  float current_mA = ina219.getCurrent_mA();
  float power_mW = ina219.getPower_mW();

  Serial.print("Bus Voltage:   "); Serial.print(busVoltage_V); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntVoltage_mV); Serial.println(" mV");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");

  delay(1000);
}
