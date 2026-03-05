#include <SPI.h>
#include <SD.h>
#include <LoRa.h>

// Shared SPI pin definitions
#define SPI_SCK   7
#define SPI_MISO  8
#define SPI_MOSI  9

// LoRa pins
#define LORA_CS   2
#define LORA_RST  3
#define LORA_DIO0 1

// SD card CS pin
#define SD_CS     21

void setupLoRa() {
  // Set CS as OUTPUT and HIGH
  pinMode(LORA_CS, OUTPUT);
  digitalWrite(LORA_CS, HIGH);

  // Initialize SPI for LoRa
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);

  // Retry until LoRa successfully initializes
  while (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa init failed. Retrying in 5 seconds...");
    delay(5000);
  }
  Serial.println("✅ LoRa init success.");

  LoRa.end();  // Disable for now
  digitalWrite(LORA_CS, HIGH); // Deselect LoRa
}

void setupSD() {
  // Set CS as OUTPUT and HIGH
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // Reinitialize SPI for SD
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  Serial.println("[SD] Initializing...");
  if (!SD.begin(SD_CS)) {
    Serial.println("❌ SD init failed. Check card or wiring.");
  } else {
    Serial.println("✅ SD initialized.");
  }
  digitalWrite(SD_CS, HIGH); // Deselect SD
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n[INIT] Starting LoRa then SD setup...");

  setupLoRa();
  delay(3000);
  setupSD();
}

void loop() {
  // ---- Use LoRa ----
  digitalWrite(SD_CS, HIGH);     // Disable SD
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, LORA_CS); // Reassign for LoRa
  digitalWrite(LORA_CS, LOW);    // Enable LoRa
  Serial.println("Now in use: LoRa");
  delay(2000);
  digitalWrite(LORA_CS, HIGH);   // Disable LoRa

  // ---- Use SD ----
  digitalWrite(SD_CS, LOW);      // Enable SD
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS); // Reassign for SD
  Serial.println("Now in use: SD");
  delay(2000);
  digitalWrite(SD_CS, HIGH);     // Disable SD
}
