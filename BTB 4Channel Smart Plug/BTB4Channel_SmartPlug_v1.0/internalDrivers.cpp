#include "internalDrivers.h"
#include "Config.h"

internalDrivers iiiiDrivers;

internalDrivers::internalDrivers() {
}

void internalDrivers::gpioInit() {
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(_relayPins[i], OUTPUT);
    delay(1);
  }
  pinMode(_statusLED, OUTPUT);
}

void internalDrivers::relayControl(char relayPin, bool state) {
  digitalWrite(relayPin, state);
}

void internalDrivers::readDataFromEEPROM() {
  if (((SSID == "") && (PASSWORD == "")) || ((ssidLength == 255) || (passwordLength == 255))) {
    EEPROM.begin(512);
    // espRestartFlag = EEPROM.read(espRestartFlagEEPROMAddr);
    ssidLength = EEPROM.read(ssidLenghtEEPROMAdd);
    Serial.print("ssidLength : ");
    Serial.println(ssidLength);
    passwordLength = EEPROM.read(passwordLenghtEEPROMAdd);
    Serial.print("passwordLength : ");
    Serial.println(passwordLength);
    SSID = loadStringFromEEPROM(ssidEEPROMAdd, ssidLength);
    PASSWORD = loadStringFromEEPROM(passEEPROMAdd, passwordLength);
  }
  for (char i = 0; i < 4; i++) {
    relayloadsStatus[i] = EEPROM.read(loadStateEEPROMAddress[i]);
    relayControl(_relayPins[i], relayloadsStatus[i]);
    delay(10);
  }
}

void internalDrivers::writeOneByteInEEPROM(int Add, byte data) {
  EEPROM.write(Add, data);
  EEPROM.commit();
}

void internalDrivers::earasWiFiCredentialsFromEEPROM() {
  Serial.println("Clearing EEORM");
  for (int i = 0; i < 43; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void internalDrivers::storeStringInEEPROM(String data, byte Addr) {
  // int ssdInitEepromAddr = 0;  // Start address in EEPROM
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
