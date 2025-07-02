#include <sys/_intsup.h>
#include "Config.h"

String SSID = "";
String PASSWORD = "";

byte MAC[6] = { 0x8C, 0x4C, 0xAD, 0xF0, 0xBF, 0x34 };
// const char* ServerMQTT = "203.109.124.70";
const char* ServerMQTT = "evoluzn.org";
int MqttPort = 18889;
const char* mqttUserName = "evzin_led";
const char* mqttUserPassword = "63I9YhMaXpa49Eb";

String devNamePrefix = "BTB";
String subTopic = "/control";  // devName + 6 digits of MAC + /Control
String pubTopic = "/status";   // // devName + 6 digits of MAC + /Status
String globalTopic = "BTBGlobal";
String apSSID = "BTBAp";
String apPassword = "123456789";
String deviceId = "";
String deviceAlertId = "";

const byte ssidEEPROMAdd = 00;
const byte passEEPROMAdd = 21;
const byte ssidLenghtEEPROMAdd = 41;
const byte passwordLenghtEEPROMAdd = 42;
const byte publishIntervalEEPROMAdd = 43;

const byte STM32_ADDRESS = 0x08;
const byte STM32_ADDRESS1 = 0x04;
const byte STM32_ADDRESS2 = 0x07;

const char _wifiStatusLED = 4;
const char pin = 27;
const char pin1 = 4;
// const char led = 2;
const char pin2 = 5;

bool responseOn200Request = false;
bool ledState = false;
bool alertFlag = false;
bool enterInAPMode = false;
bool globallySSIDAndPasswordChange = false;
bool espRestartFlag = false;
bool isApStarted = false;
bool isWifiConnected = false;

unsigned int previousTime = 0;

byte ssidLength = 0;
byte passwordLength = 0;
byte wiFiRetryCounter = 0;
byte mqttCounter = 0;
byte responseLength = 2;
byte interval = 60;

String alertMsg = "";
