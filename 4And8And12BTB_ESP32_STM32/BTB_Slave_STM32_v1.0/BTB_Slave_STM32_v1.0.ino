#include "CSE7759.h"
#include <Wire.h>
#define SLAVE_ADDRESS 0x08
const char* Cmsg;

CSE7759 cse7759;
PowerData powerData;

void setup() {
  Wire.begin(SLAVE_ADDRESS);  // Join I2C bus as slave with address 0x08
  Serial.begin(9600);         // for debugging
  Serial1.begin(4800);
  cse7759.initialize_power_data(&powerData);
  Wire.onRequest(requestEvent);
}
void loop() {
  powerData = cse7759.validateTheCSEChipData(cse7759.read_power_data());
  String msg = cse7759.powerDataToString(powerData);
  // Cmsg = msg.c_str();

 // Serial.println(Cmsg);
//  String mergeData =  String result = "";
//   result += String(data.voltage, 2) + ":";
//   result += String(data.current, 3) + ":";
//   result += String(data.power, 2);
//   return result;
  delay(1000);
}

void requestEvent() {
  Wire.write((const uint8_t*)Cmsg, strlen(Cmsg));
}