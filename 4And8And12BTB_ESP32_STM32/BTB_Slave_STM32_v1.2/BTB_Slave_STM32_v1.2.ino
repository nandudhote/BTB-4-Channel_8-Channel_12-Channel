#include "CSE7759.h"
#include <Wire.h>
#include <EEPROM.h>

#define SLAVE_ADDRESS 0x07

extern const uint8_t _relayPins[] = { PA1, PA2, PA3, PA4 };
extern const byte loadStateEEPROMAddress[] = { 0, 1, 2, 3 };
extern bool relayloadsStatus[] = { 0, 0, 0, 0 };

String powerDataMsg = "";
String loadMsg = "1";


CSE7759 cse7759;
PowerData powerData;

const int pin = PB0;

void setup() {
  Wire.begin(SLAVE_ADDRESS);  // Join I2C bus as slave with address 0x08
  Serial.begin(9600);         // for debugging
  Serial1.begin(4800);
  gpioInit();
  cse7759.initialize_power_data(&powerData);
  readDataFromEEPROM();
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}
void loop() {
  powerData = cse7759.validateTheCSEChipData(cse7759.read_power_data());
  powerDataMsg = powerDataToString(powerData);
  Serial.println(powerDataMsg);
  delay(1000);
}

void gpioInit() {
  pinMode(pin, INPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(_relayPins[i], INPUT);
    delay(10);
  }
}

void requestEvent() {
  Wire.write(powerDataMsg.c_str(), strlen(powerDataMsg.c_str()));
  Wire.write(loadMsg.c_str(), strlen(loadMsg.c_str()));
  Serial.println("Data Sent ");
}

void receiveEvent(int numBytes) {
  String receivedData = "";  // Clear buffer
  while (Wire.available()) {
    char c = Wire.read();
    receivedData += c;
  }
  Serial.print("Received from master: ");
  Serial.println(receivedData);

  SplitData i2cDecodedMsg = cse7759.splitStringByColon(receivedData);

  bool state = i2cDecodedMsg.indexTwoData.toInt();
  if (i2cDecodedMsg.indexOneData == "1") {
    relayControl(_relayPins[0], state);
    relayloadsStatus[0] = state;
    writeOneByteInEEPROM(loadStateEEPROMAddress[0], state);
  } else if (i2cDecodedMsg.indexOneData == "2") {
    relayControl(_relayPins[1], state);
    relayloadsStatus[1] = state;
    writeOneByteInEEPROM(loadStateEEPROMAddress[1], state);
  } else if (i2cDecodedMsg.indexOneData == "3") {
    relayControl(_relayPins[2], state);
    relayloadsStatus[2] = state;
    writeOneByteInEEPROM(loadStateEEPROMAddress[2], state);
  } else if (i2cDecodedMsg.indexOneData == "4") {
    relayControl(_relayPins[3], state);
    relayloadsStatus[3] = state;
    writeOneByteInEEPROM(loadStateEEPROMAddress[3], state);
  } else if (i2cDecodedMsg.indexOneData == "slaveRestart") {
    NVIC_SystemReset();  // Perform system reset
  } else {
    delay(1);
  }
}

void relayControl(int relayPin, bool state) {
  digitalWrite(relayPin, state);
}

void readDataFromEEPROM() {
  for (int i = 0; i < 4; i++) {
    relayloadsStatus[i] = EEPROM.read(loadStateEEPROMAddress[i]);
    relayControl(_relayPins[i], relayloadsStatus[i]);
    delay(10);
  }
}

void writeOneByteInEEPROM(int Add, byte data) {
  EEPROM.write(Add, data);
}


String powerDataToString(const PowerData& data) {
  String result = "";
  result += String(data.voltage, 2) + ":";
  result += String(data.current, 2) + ":";
  result += String(data.power, 2) + ":";
  result += String(relayloadsStatus[0]) + ":";
  result += String(relayloadsStatus[1]) + ":";
  result += String(relayloadsStatus[2]) + ":";
  result += String(relayloadsStatus[3]);
  return result;
}
