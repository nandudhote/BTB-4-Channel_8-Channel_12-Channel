#include "CSE7759_one.h"
#include "CSE7759_two.h"
#include <Wire.h>
#include <EEPROM.h>

#define SLAVE_ADDRESS 0x07

extern const uint8_t _relayPins[] = { PA1, PA2, PA3, PA4 };
extern const byte loadStateEEPROMAddress[] = { 0, 1, 2, 3 };
extern bool relayloadsStatus[] = { 0, 0, 0, 0 };

String powerDataMsg = "";
String loadMsg = "1";

struct SplitData {
  String indexOneData;
  String indexTwoData;
  String indexThreeData;
};

HardwareSerial Serial2(PB10, PB11);  // TX, RX for USART3 on STM32F1


CSE7759_one cse7759_one;
PowerData1 powerData1;
CSE7759_two cse7759_two;
PowerData2 powerData2;

const int pin = PB0;

void setup() {
  Wire.begin(SLAVE_ADDRESS);  // Join I2C bus as slave with address 0x08
  Serial.begin(9600);         // for debugging
  Serial1.begin(4800);
  Serial2.begin(4800);
  gpioInit();
  cse7759_one.initialize_power_data_one(&powerData1);
  cse7759_two.initialize_power_data_two(&powerData2);
  readDataFromEEPROM();
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
}
void loop() {
  powerData1 = cse7759_one.validateTheCSEChipData_one(cse7759_one.read_power_data_one());
  powerData2 = cse7759_two.validateTheCSEChipData_two(cse7759_two.read_power_data_two());
  powerDataMsg = powerDataToString(powerData1, powerData2);
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

  SplitData i2cDecodedMsg = splitStringByColon(receivedData);

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


String powerDataToString(const PowerData1& data1, const PowerData2& data2) {
  String result = "";
  result += String(data1.voltage1, 2) + ":";
  result += String(data1.current1, 2) + ":";
  result += String(data1.power1, 2) + ":";
  result += String(data2.voltage2, 2) + ":";
  result += String(data2.current2, 2) + ":";
  result += String(data2.power2, 2) + ":";
  result += String(relayloadsStatus[0]) + ":";
  result += String(relayloadsStatus[1]) + ":";
  result += String(relayloadsStatus[2]) + ":";
  result += String(relayloadsStatus[3]);
  return result;
}

SplitData splitStringByColon(const String& data) {
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
