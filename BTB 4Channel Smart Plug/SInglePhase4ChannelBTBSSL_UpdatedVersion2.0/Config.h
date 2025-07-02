#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"

extern String SSID;
extern String PASSWORD;

extern byte MAC[6];
extern const char* ServerMQTT;
extern int MqttPort;
// extern const char* mqttUserName;
// extern const char* mqttUserPassword;

extern String devNamePrefix;
extern String subTopic;  // devName + 6 digits of MAC + /Control
extern String pubTopic;
extern String globalTopic;
extern String apSSID;
extern String apPassword;
extern String deviceId;
extern String alertMsg;

extern const byte ssidEEPROMAdd;
extern const byte passEEPROMAdd;
extern const byte ssidLenghtEEPROMAdd;
extern const byte passwordLenghtEEPROMAdd;
extern const byte publishIntervalEEPROMAdd;
extern const byte loadStateEEPROMAddress[];

extern const char _relayPins[];
extern const char _statusLED;

extern bool responseOn200Request;
extern bool relayloadsStatus[];
extern bool enterInAPMode;
extern bool globallySSIDAndPasswordChange;
extern bool isApStarted;

extern unsigned int previousTime;

extern float voltagePre;
extern float currentPre;
extern float powerPre;

extern byte ssidLength;
extern byte passwordLength;
extern byte wiFiRetryCounter;
extern byte mqttCounter;
extern byte interval;


#endif
