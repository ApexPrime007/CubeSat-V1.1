#include <Wire.h>
#include <SPI.h>

// ==== ESP32 I2C PINS (change if needed) ====
#define SDA_PIN 5
#define SCL_PIN 6

// ==== LoRa SPI PINS (adjust to your wiring) ====
#define LORA_SS   1
#define LORA_RST  2
#define LORA_DIO0 3

// Known I2C addresses
#define MPU6050_ADDR   0x68
#define HMC5883L_ADDR  0x1E
#define BMP280_ADDR1   0x76
#define BMP280_ADDR2   0x77
#define ADS1115_ADDR1  0x48
#define ADS1115_ADDR2  0x49
#define ADS1115_ADDR3  0x4A
#define ADS1115_ADDR4  0x4B
#define MCP23017_ADDR1 0x20
#define MCP23017_ADDR2 0x21
#define MCP23017_ADDR3 0x22
#define MCP23017_ADDR4 0x23
#define MCP23017_ADDR5 0x24
#define MCP23017_ADDR6 0x25
#define MCP23017_ADDR7 0x26
#define MCP23017_ADDR8 0x27

void scanI2C() {
  Serial.println("\nScanning I2C bus...");
  byte count = 0;

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Device found at 0x");
      Serial.println(address, HEX);
      count++;
    }
  }

  if (count == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("Scan complete.");
}

bool checkDevice(byte address) {
  Wire.beginTransmission(address);
  return (Wire.endTransmission() == 0);
}

void checkSpecificDevices() {
  Serial.println("\nChecking known devices:");

  if (checkDevice(MPU6050_ADDR))
    Serial.println("MPU6050 detected!");

  if (checkDevice(HMC5883L_ADDR))
    Serial.println("HMC5883L detected!");

  if (checkDevice(BMP280_ADDR1) || checkDevice(BMP280_ADDR2))
    Serial.println("BMP280 detected!");

  if (checkDevice(ADS1115_ADDR1) || checkDevice(ADS1115_ADDR2) ||
      checkDevice(ADS1115_ADDR3) || checkDevice(ADS1115_ADDR4))
    Serial.println("ADS1115 detected!");

  if (checkDevice(MCP23017_ADDR1) || checkDevice(MCP23017_ADDR2) ||
      checkDevice(MCP23017_ADDR3) || checkDevice(MCP23017_ADDR4) ||
      checkDevice(MCP23017_ADDR5) || checkDevice(MCP23017_ADDR6) ||
      checkDevice(MCP23017_ADDR7) || checkDevice(MCP23017_ADDR8))
    Serial.println("MCP23017 detected!");
}

// Basic SPI LoRa presence check (SX127x register read)
void checkLoRa() {
  Serial.println("\nChecking LoRa (SX127x)...");

  pinMode(LORA_SS, OUTPUT);
  digitalWrite(LORA_SS, HIGH);

  SPI.begin();
  digitalWrite(LORA_SS, LOW);
  SPI.transfer(0x42 & 0x7F);  // RegVersion
  byte version = SPI.transfer(0x00);
  digitalWrite(LORA_SS, HIGH);

  if (version == 0x12) {
    Serial.println("LoRa SX127x detected!");
  } else {
    Serial.print("LoRa not detected. Version read: 0x");
    Serial.println(version, HEX);
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  Wire.begin(SDA_PIN, SCL_PIN);

  scanI2C();
  checkSpecificDevices();
  checkLoRa();
}

void loop() {
  delay(10000);  // Repeat every 10 seconds
}