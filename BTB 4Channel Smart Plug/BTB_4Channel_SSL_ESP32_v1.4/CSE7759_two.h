#ifndef CSE7759_two_H
#define CSE7759_two_H

#include "Arduino.h"
#include <string.h>
#include <math.h>

typedef struct {
  float voltage;
  float current;
  float power;
  char hex[30];
  unsigned int readings[24];
  unsigned int checksum;
  int test_flag;
  bool ifDataIsOk;
} PowerData2;

class CSE7759_two {
public:
  CSE7759_two();
  void initialize_power_data_two(PowerData2* data);
  int hex_to_dec_two(const char* hex);
  void prepare_hex_two(unsigned int byte1, unsigned int byte2, unsigned int byte3, char* hex);
  void calculate_power_two(PowerData2* data);
  PowerData2 read_power_data_two();
  PowerData2 validateTheCSEChipData_two(PowerData2 powerData);
  void addSamplesForPower(float value3);
  float getAverageForPower();
  void addSamplesForCurrent(float value4);
  float getAverageForCurrent();
  void addSamplesForVoltage(float value5);
  float getAverageForVoltage();
};

#endif
