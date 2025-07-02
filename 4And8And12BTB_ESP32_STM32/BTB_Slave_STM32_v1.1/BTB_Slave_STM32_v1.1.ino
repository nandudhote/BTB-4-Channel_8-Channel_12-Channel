#include "CSE7759.h"
#include <Wire.h>
#define SLAVE_ADDRESS 0x07

//const char* Cmsg;
String msg = "";
String msg1 = "SmitBhaiya";


CSE7759 cse7759;
PowerData powerData;


const int pin = PB0;

void setup() {
  Wire.begin(SLAVE_ADDRESS);  // Join I2C bus as slave with address 0x08
  Serial.begin(9600);         // for debugging
  Serial1.begin(4800);
  cse7759.initialize_power_data(&powerData);
  pinMode(pin, INPUT);
  Wire.onRequest(requestEvent);
}
void loop() {
  powerData = cse7759.validateTheCSEChipData(cse7759.read_power_data());
  msg = cse7759.powerDataToString(powerData);
  // msg = "Hello Master 0x08";
  // Cmsg = msg.c_str();
  Serial.println(msg);
  delay(1000);
}

void requestEvent() {
  int state = digitalRead(pin);
  Serial.println(state);
  if (state == 1) {
  Wire.write(msg.c_str(), strlen(msg.c_str()));
  Wire.write(msg1.c_str(), strlen(msg1.c_str()));
  Serial.println("Data Sent ");
  }
  if(state == 0 ){
    while(1)
    {
      int state1 = digitalRead(pin);
      Serial.println("in while");
      if(state1 == 1){
        loop();
      }
    }
  }
}
