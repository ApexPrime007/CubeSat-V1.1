void setup() {
  Serial.begin(115200);   // Debug monitor
  Serial1.begin(9600);    // GPS default baud
  Serial.println("Raw GPS data test...");
}

void loop() {
  while (Serial1.available()) {
    char c = Serial1.read();
    Serial.write(c);   // Echo to monitor
  }
}
