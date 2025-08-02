#ifndef CSE7759_one_H
#define CSE7759_one_H

#include "Arduino.h"
#include <string.h>
#include <math.h>

typedef struct {
  float voltage1;
  float current1;
  float power1;
  char hex1[30];
  unsigned int readings[24];
  unsigned int checksum;
  int test_flag1;
  bool ifDataIsOk1;
} PowerData1;

class CSE7759_one {
public:
  CSE7759_one();
  void initialize_power_data_one(PowerData1* data);
  int hex_to_dec_one(const char* hex);
  void prepare_hex_one(unsigned int byte1, unsigned int byte2, unsigned int byte3, char* hex);
  void calculate_power_one(PowerData1* data);
  PowerData1 read_power_data_one();
  PowerData1 validateTheCSEChipData_one(PowerData1 powerData1);
};

#endif
