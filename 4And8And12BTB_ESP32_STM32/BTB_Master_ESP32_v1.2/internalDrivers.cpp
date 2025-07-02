#include "internalDrivers.h"
#include "Config.h"
#include "myWIFI.h"
#include <Wire.h>

myWIFI iwiFI;

internalDrivers::internalDrivers() {
}

void internalDrivers::gpioInit() {
  Wire.begin();  // Master mode, default SDA/SCL
  pinMode(_wifiStatusLED, OUTPUT);
  pinMode(pin, OUTPUT);
  pinMode(pin1, OUTPUT);
  // pinMode(led, OUTPUT);
  pinMode(pin2, OUTPUT);
}

void internalDrivers::readDataFromEEPROM() {
  EEPROM.begin(512);
  ssidLength = EEPROM.read(ssidLenghtEEPROMAdd);
  passwordLength = EEPROM.read(passwordLenghtEEPROMAdd);
  SSID = loadStringFromEEPROM(ssidEEPROMAdd, ssidLength);
  PASSWORD = loadStringFromEEPROM(passEEPROMAdd, passwordLength);
  byte intervalTemp = EEPROM.read(publishIntervalEEPROMAdd);
  if (intervalTemp != 0) {
    interval = intervalTemp;
  }
}

void internalDrivers::i2cScanner() {
  byte error, address;
  int nDevices = 0;
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

void internalDrivers::requestDataFromSlave() {
  String publishData = "";
  digitalWrite(pin, 1);
  Serial.println("pin High");
  Wire.requestFrom(STM32_ADDRESS, 20);
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
  Wire.requestFrom(STM32_ADDRESS1, 20);
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
  Wire.requestFrom(STM32_ADDRESS2, 20);
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
  publishData = received + ":" + received1 + ":" + received2;
  iwiFI.checkMqttConnectionAndPublishData(publishData);
}


void internalDrivers::sendDataToSlave(byte address, String data) {
  Wire.beginTransmission(address);
  Wire.write((const uint8_t*)data.c_str(), data.length());  // Corrected line
  Wire.endTransmission();
}


void internalDrivers::writeOneByteInEEPROM(int Add, byte data) {
  EEPROM.write(Add, data);
  EEPROM.commit();
}

void internalDrivers::storeStringInEEPROM(String data, byte Addr) {
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(Addr + i, data.charAt(i));
  }
  EEPROM.commit();
}

String internalDrivers::loadStringFromEEPROM(byte Addr, byte Length) {
  String readData = "";
  for (int i = Addr; i < (Addr + Length); i++) {
    readData += char(EEPROM.read(i));
  }
  return readData;
}

SplitData internalDrivers::splitStringByColon(const String& data) {
  SplitData mqttMsg;
  int firstIndex = data.indexOf(':');
  if (firstIndex != -1) {
    mqttMsg.indexOneData = data.substring(0, firstIndex);
    int secondIndex = data.indexOf(':', firstIndex + 1);
    if (secondIndex != -1) {
      mqttMsg.indexTwoData = data.substring(firstIndex + 1, secondIndex);
      mqttMsg.indexThreeData = data.substring(secondIndex + 1);
      if (mqttMsg.indexThreeData.length() > 0) {
      }
    } else {
      mqttMsg.indexTwoData = data.substring(firstIndex + 1);
    }
  } else {
    mqttMsg.indexOneData = data.substring(firstIndex + 1);
  }
  return mqttMsg;
}