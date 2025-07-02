#include "Config.h"
#include "internalDrivers.h"

String SSID = "";
String PASSWORD = "";

byte MAC[6] = { 0x8C, 0x4C, 0xAD, 0xF0, 0xBF, 0x33 };  // 8C:4C:AD:F0:BE:83
//const char* ServerMQTT = "mqtt.evoluzn.in";
// const char* ServerMQTT = "evoluzn.org";
// int MqttPort = 18889;
const char* ServerMQTT = "a6kvi1np2cmrt-ats.iot.ap-south-1.amazonaws.com";
int MqttPort = 8883;
// const char* mqttUserName = "evzin_led";
// const char* mqttUserPassword = "63I9YhMaXpa49Eb";

String devNamePrefix = "BTB4Channel";
String subTopic = "/control";  // devName + 6 digits of MAC + /Control
String pubTopic = "/status";   // // devName + 6 digits of MAC + /Status
String globalTopic = "BTB4ChannelGlobal";
String apSSID = "BTB4ChannelAp";
String apPassword = "123456789";
String deviceId = "";
String alertMsg = "";

const byte ssidEEPROMAdd = 00;
const byte passEEPROMAdd = 21;
const byte ssidLenghtEEPROMAdd = 41;
const byte passwordLenghtEEPROMAdd = 42;
const byte publishIntervalEEPROMAdd = 48;
const byte loadStateEEPROMAddress[] = { 44, 45, 46, 47 };

const char _relayPins[] = { 5, 4, 14, 12 };  // D1, D2, D5, D6
const char _statusLED = 13;                  //D7

bool responseOn200Request = false;
bool relayloadsStatus[] = { 0, 0, 0, 0 };
bool enterInAPMode = false;
bool globallySSIDAndPasswordChange = false;
bool espRestartFlag = false;
bool isApStarted = false;

unsigned int previousTime = 0;

byte ethernetResetCounter = 0;
byte ssidLength = 0;
byte passwordLength = 0;
byte wiFiRetryCounter = 0;
byte mqttCounter = 0;
byte interval = 60;

float voltagePre = 0.0;
float currentPre = 0.0;
float powerPre = 0.0;
