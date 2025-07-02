#include <Wire.h>

#define STM32_ADDRESS 0x07

void setup() {
  Serial.begin(115200);
  Wire.begin();  // Master mode, default SDA/SCL
}

void loop() {

  // Request 3 bytes from STM32
  Wire.requestFrom(STM32_ADDRESS, 17);

  String received = "";
  while (Wire.available()) {
    char c = Wire.read();
    received += c;
  }

  if (received.length() > 0) {
    Serial.print("Data received from STM32: ");
    Serial.println(received);
  }
  delay(1000);
}
