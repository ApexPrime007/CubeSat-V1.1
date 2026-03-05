#include <SPI.h>
#include <LoRa.h>

// --- ESP32 DevKit V1  (SX1278 @ 433 MHz) ---
#define LORA_SCK    18    // VSPI SCK
#define LORA_MISO   19    // VSPI MISO
#define LORA_MOSI   23    // VSPI MOSI
#define LORA_CS      5    // NSS / CS
#define LORA_RST    14    // RESET
#define LORA_DIO0   26    // DIO0 (IRQ)

#define LORA_FREQ  433E6  // Must match transmitter
#define SYNC_WORD  0x34   // Must match transmitter (Arduino LoRa default)

// --- Optional modem params (match TX) ---
#define SF   7            // Spreading Factor (default 7)
#define BW   125E3        // Bandwidth (default 125 kHz)
#define CR4  5            // Coding rate denominator (default 5 -> 4/5)

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\nLoRa Receiver — ESP32 DevKit V1");

  // Init SPI first
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI);

  // Tell LoRa lib our control pins
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);

  // Bring up the radio
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Starting LoRa failed! Check wiring/freq.");
    return; // avoid while(1)
  }

  // Make sure RX settings match the transmitter
  LoRa.setSyncWord(SYNC_WORD);   // Keep same on TX & RX
  LoRa.setSpreadingFactor(SF);   // 6..12
  LoRa.setSignalBandwidth(BW);   // 7.8k..500k
  LoRa.setCodingRate4(CR4);      // 5..8 -> 4/5..4/8
  LoRa.enableCrc();              // Enable CRC if TX uses it (Arduino LoRa defaults to CRC OFF; set on TX too if you enable here)

  // Optional preamble (match TX if you change it)
  // LoRa.setPreambleLength(8);

  // Put into continuous receive mode
  LoRa.receive();

  Serial.println("LoRa init OK. Listening for packets...");
}

void loop() {
  // Non-blocking packet check
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("--------------------------------------------------");
    Serial.print("Packet bytes: "); Serial.println(packetSize);

    // Read the payload
    String msg;
    while (LoRa.available()) {
      msg += (char)LoRa.read();
    }
    Serial.print("Payload: ");
    Serial.println(msg);

    // Link diagnostics
    Serial.print("RSSI: ");
    Serial.print(LoRa.packetRssi());
    Serial.print(" dBm,  SNR: ");
    Serial.print(LoRa.packetSnr());
    Serial.println(" dB");

    // If you enabled CRC above, you can check validity via implicit failure to parse
    // (library silently discards bad CRC packets). If you want to see CRC errors,
    // keep CRC disabled or use a modified lib.

    // Return to RX mode (defensive; parsePacket usually does this)
    LoRa.receive();
  }

  // Light heartbeat so you know it's alive
  static uint32_t lastBeat = 0;
  if (millis() - lastBeat > 2000) {
    Serial.println("...listening");
    lastBeat = millis();
  }
}
