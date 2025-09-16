#include <sys/_intsup.h>
#include "Config.h"

String SSID = "";
String PASSWORD = "";

byte MAC[6] = { 0x8C, 0x4C, 0xAD, 0xF0, 0xBF, 0xE1};
// const char* ServerMQTT = "mqtt.evoluzn.in";
// const char* ServerMQTT = "evoluzn.org";
const char* ServerMQTT = "a6kvi1np2cmrt-ats.iot.ap-south-1.amazonaws.com";
int MqttPort = 8883;
// int MqttPort = 18889;
const char* mqttUserName = "evzin_led";
const char* mqttUserPassword = "63I9YhMaXpa49Eb";

String devNamePrefix = "BTB4Channel";
String subTopic = "/control";  // devName + 6 digits of MAC + /Control
String pubTopic = "/status";   // // devName + 6 digits of MAC + /Status
String globalTopic = "BTBGlobal";
String apSSID = "BTBAp";
String apPassword = "123456789";
String deviceId = "";

const byte ssidEEPROMAdd = 00;
const byte passEEPROMAdd = 21;
const byte ssidLenghtEEPROMAdd = 41;
const byte passwordLenghtEEPROMAdd = 42;
const byte publishIntervalEEPROMAdd = 43;
const byte autoMotionFlagEEPROMAddr = 44;
const byte motionCounterMsgEEPROM = 45;
const byte motionDetectThresholdMsgEEPROM = 46;
const byte globalTopicLenghtEEPROMAdd = 47;
const byte globalTopicEEPROMAdd = 52;  // Global topic max length should of 22 bits

const byte loadStateEEPROMAddress[] = { 48, 49, 50, 51 };

const char _wifiStatusLED = 12;
const char _PIRSensorPin = 13;
const char _relayPins[] = { 17, 18, 19, 21 };

//const char _relayPins[] = { 21, 19, 18, 17 };

bool relayloadsStatus[] = { 0, 0, 0, 0 };

bool responseOn200Request = false;
bool enterInAPMode = false;
bool globallySSIDAndPasswordChange = false;
bool isApStarted = false;
bool isWifiConnected = false;

unsigned int previousTime = 0;

byte ssidLength = 0;
byte passwordLength = 0;
byte wiFiRetryCounter = 0;
byte mqttCounter = 0;
byte responseLength = 2;
byte interval = 60;
byte globalTopicLength = 0;

String alertMsg = "";

float voltagePre = 0.0;
float currentPre = 0.0;
float powerPre = 0.0;

float voltagePre2 = 0.0;
float currentPre2 = 0.0;
float powerPre2 = 0.0;

// int duration = 0;
bool autoMotionDetect_Flag = false;
unsigned int motionCounter = 30;  // Time to keep the load ON after detecting motion (in seconds)
int motionDetectCount = 0;        // Counter to count consecutive motion detections
int motionDetectThreshold = 2;    // Threshold: How many motion detections are needed to trigger the relay
unsigned int onTimer = 0;         // Timer that keeps track of how long the load has been ON
bool motionDetected = false;      // Flag to track whether motion was detected and relay is ON
// unsigned int motionConfirmedCounter = motionCounter;
// int PIR_THRESH = 200;

int indexForPowerCSE1 = 0;
bool bufferFilledForPowerCSE1 = false;
float samplesForPowerCSE1[] = { 0.00, 0.00, 0.00, 0.00, 0.00 };

// int indexForCurrentCSE1 = 0;
// bool bufferFilledForCurrentCSE1 = false;
// float samplesForCurrentCSE1[] = { 0.00, 0.00, 0.00, 0.00, 0.00 };

// int indexForVoltageCSE1 = 0;
// bool bufferFilledForVoltageCSE1 = false;
// float samplesForVoltageCSE1[] = { 0.00, 0.00, 0.00, 0.00, 0.00 };

int indexForPowerCSE2 = 0;
bool bufferFilledForPowerCSE2 = false;
float samplesForPowerCSE2[] = { 0.00, 0.00, 0.00, 0.00, 0.00 };

// int indexForCurrentCSE2 = 0;
// bool bufferFilledForCurrentCSE2 = false;
// float samplesForCurrentCSE2[] = { 0.00, 0.00, 0.00, 0.00, 0.00 };

// int indexForVoltageCSE2 = 0;
// bool bufferFilledForVoltageCSE2 = false;
// float samplesForVoltageCSE2[] = { 0.00, 0.00, 0.00, 0.00, 0.00 };
