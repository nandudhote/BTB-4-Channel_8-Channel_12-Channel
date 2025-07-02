#include <Wire.h>
#define STM32_ADDRESS 0x08
#define STM32_ADDRESS1 0x04
#define STM32_ADDRESS2 0x07

const int pin = 27;
const int pin1 = 4;
const int led = 2;
const int pin2 = 5;

void setup() {
  delay(5000);
  Serial.begin(115200);
  pinMode(pin, OUTPUT);
  pinMode(pin1, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(pin2, OUTPUT);

  Wire.begin();  // Master mode, default SDA/SCL
  byte error, address;
  int nDevices = 0;

  // delay(5000);

  Serial.println("Scanning for I2C devices ...");
  for (address = 0x01; address < 0x7f; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.printf("I2C device found at address 0x%02X\n", address);
      nDevices++;
    } else if (error != 2) {
      Serial.printf("Error %d at address 0x%02X\n", error, address);
    }
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found");
  }
}

void loop() {
  digitalWrite(pin, 1);
  Serial.println("pin High");
  Wire.requestFrom(STM32_ADDRESS, 17);
  String received = "";

  while (Wire.available()) {
    char c = Wire.read();
    received += c;
  }

  if (received.length() > 0) {
    Serial.print("Data received from STM32 0: ");
    Serial.println(received);
    digitalWrite(pin, 0);
    Serial.println("pin Low");
  }
  delay(1000);

  digitalWrite(pin1, 1);
  Serial.println("pin1 High");
  Wire.requestFrom(STM32_ADDRESS1, 17);
  String received1 = "";

  while (Wire.available()) {
    char c = Wire.read();
    received1 += c;
  }

  if (received1.length() > 0) {
    Serial.print("Data received from STM32 0: ");
    Serial.println(received1);
    digitalWrite(pin1, 0);
    Serial.println("pin1 Low");
  }

  delay(1000);

  digitalWrite(pin2, 1);
  Serial.println("pin2 High");
  Wire.requestFrom(STM32_ADDRESS2, 17);
  String received2 = "";

  while (Wire.available()) {
    char c = Wire.read();
    received2 += c;
  }

  if (received2.length() > 0) {
    Serial.print("Data received from STM32 0: ");
    Serial.println(received2);
    digitalWrite(pin2, 0);
    Serial.println("pin2 Low");
  }


  digitalWrite(led, 1);
  Serial.println("led On");
  delay(3000);
  digitalWrite(led, 0);
  Serial.println("led OFF");
  // digitalWrite(pin1, 0);
  delay(3000);

  
  delay(1000);
}
