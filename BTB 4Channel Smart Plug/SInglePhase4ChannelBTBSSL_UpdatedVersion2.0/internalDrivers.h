#ifndef INTERNALDRIVERS_H
#define INTERNALDRIVERS_H

#include "Arduino.h"
#include <EEPROM.h>

//DateTime startTime, endTime;
typedef struct VolCurPow {
  double voltage, current, power, voltagePrev, currentPrev, powerPrev;
} V_I_P;

struct SplitData {
  String indexOneData;
  String indexTwoData;
  String indexThreeData;
};

class internalDrivers {
public:
  internalDrivers();
  void gpioInit();
  void relayControl(char relaypin , bool state);
  void readDataFromEEPROM();
  void writeOneByteInEEPROM(int Add, byte data);
  void earasWiFiCredentialsFromEEPROM();
  void storeStringInEEPROM(String data, byte Addr);
  String loadStringFromEEPROM(byte Addr, byte Length);
  SplitData splitStringByColon(const String& data);
};

#endif
