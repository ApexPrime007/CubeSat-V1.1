#include <SPI.h>
#include <LoRa.h>

// --- XIAO ESP32S3 wiring (adjust if different) ---
#define LORA_SCK    7     // VSPI SCK
#define LORA_MISO   8     // VSPI MISO
#define LORA_MOSI   9     // VSPI MOSI
#define LORA_CS     1     // NSS / CS
#define LORA_RST    2     // RESET
#define LORA_DIO0   3     // DIO0 (IRQ)

#define LORA_FREQ  433E6  // Must match receiver
#define SYNC_WORD  0x34   // Must match receiver

// --- Modem params (match RX) ---
#define SF   7            // Spreading Factor (default 7)
#define BW   125E3        // Bandwidth (default 125 kHz)
#define CR4  5            // Coding rate denominator (default 5 -> 4/5)

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\nLoRa Transmitter — XIAO ESP32S3");

  // Init SPI first
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI);

  // Tell LoRa lib our control pins
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);

  // Bring up the radio
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Starting LoRa failed! Check wiring/freq.");
    while (1);
  }

  // Match RX settings
  LoRa.setSyncWord(SYNC_WORD);
  LoRa.setSpreadingFactor(SF);
  LoRa.setSignalBandwidth(BW);
  LoRa.setCodingRate4(CR4);
  LoRa.enableCrc();

  LoRa.setTxPower(17);   // 2..20 dBm depending on module

  Serial.println("LoRa init OK. Ready to transmit.");
}

void loop() {
  static uint32_t counter = 0;

  Serial.print("Sending packet #");
  Serial.println(counter);

  LoRa.beginPacket();
  LoRa.print("hello from XIAO ESP32S3, packet ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(2000);  // match RX heartbeat period
}
