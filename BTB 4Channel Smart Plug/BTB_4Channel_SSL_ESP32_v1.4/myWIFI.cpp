#include "myWiFi.h"       // Include custom WiFi management header
#include "Config.h"       // Include configuration constants
#include "accessPoint.h"  // Include Access Point handling class
#include "internalDrivers.h"
#include <EEPROM.h>  // Include EEPROM library to read/write non-volatile memory

// Certificates: Use raw string literals
const char* root_ca = R"(-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----)";

const char* client_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIICtzCCAZ8CFAMWKtWNMeiwoOcdNsrDbeyVuOnpMA0GCSqGSIb3DQEBCwUAMBYx
FDASBgNVBAMMC0lPVC1ST09ULUNBMB4XDTI0MTIwNDEzMTIzOVoXDTI2MDQxODEz
MTIzOVowGjEYMBYGA1UEAwwPMjM3NzQ3MzM4NjE2MDczMIIBIjANBgkqhkiG9w0B
AQEFAAOCAQ8AMIIBCgKCAQEAqCtoLCycHufNh5/RLGWpB0ikRjY4bu03VrsoTeOR
HDq8Q28zDfK/4//43vkWkVIT3zkvNSVgsM0luWi0zi26mZ/qiIHf/mXALJjogqxU
7hLtGyH3VidXUGYDGluQbzUEOFBqmxrgmr0tkBCdpQr0tiAs0RFo2R3C6a8lwEBW
0sIC5K4EmuWpuxApQ+L96tXFYvLcGyW8niMFBMXnGVg5QfZjjWQ2KhjTu4DdAFX1
KXz4jxJrnd6hqbisioGweiWvKK4Qv+ZiEn5DQn3cPGrWW/K1EvMxPQg41djEgb8x
oaFL/ppyuxhKBYzarvbTd118zW/TU1+D++A3FUMri5y/lwIDAQABMA0GCSqGSIb3
DQEBCwUAA4IBAQCNIVVzO//qZAbcwimyyib+PL23+0hkJy1ECfiJPzMX0V58R2f+
ZnLXrd9BUfBGlvbMHWfRx/KWjvqd3FhXf6Rxx5OfICTnRAR3xKKEqhfHbBoLj1vh
igwu7IrQhQqLc6mhUTgr58XZbXnHY+rSlmGWxcS+D2uBZy9jYSq6Vv++RLErKPCp
RU5R/zWArSZngZe3hT7yfvzAvI8+O2CiJnuH9aaOdP9OmhKopIr/JGZAW8utyvmq
Zr7bWN4jDMvAxDwMA8PL5k0lDXrTGGB4wqebpYsurYCm7wtjH+QWbPZgUmu8DrRf
CPMjwlPux/wFKX3P0A4/U8AdeRWKU9dkcS3V
-----END CERTIFICATE-----
)EOF";

const char* client_key = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEAqCtoLCycHufNh5/RLGWpB0ikRjY4bu03VrsoTeORHDq8Q28z
DfK/4//43vkWkVIT3zkvNSVgsM0luWi0zi26mZ/qiIHf/mXALJjogqxU7hLtGyH3
VidXUGYDGluQbzUEOFBqmxrgmr0tkBCdpQr0tiAs0RFo2R3C6a8lwEBW0sIC5K4E
muWpuxApQ+L96tXFYvLcGyW8niMFBMXnGVg5QfZjjWQ2KhjTu4DdAFX1KXz4jxJr
nd6hqbisioGweiWvKK4Qv+ZiEn5DQn3cPGrWW/K1EvMxPQg41djEgb8xoaFL/ppy
uxhKBYzarvbTd118zW/TU1+D++A3FUMri5y/lwIDAQABAoIBAQChDIDE3SveBQu1
eH3vUKQhD5YSj1ZYFODMNIwSm36aVD17LwDruK+//6Esl8re/IV/XtowwUzpxGz+
NKICWZS8DoOimw4SKQtzKjRw/+yN2p4rP8oIPcDGhAswsMOPXGxgKdMOGx1B0y3R
K7htGCLmT8XbqhqI4pGJDWzPwk3jos7IiR92RelTzojAFP+sUTj14fYvcqM1j0nx
1hlbqneDTcFHdkjVlY2B+TLXVmtCLMPUzQfVt+nYs5T6S4dhuWAkiXgIo5vdjeXU
PT1FPD/5oBuC7Cx8bfZqzOTnKPfTVrjlHl02YdrdxR4UNjtoESfZZGrgA7gnVlNH
EUPmE4LRAoGBANzLnkgrsvA3w/+GWylsQL42RfBLRPL8vhk/xS+/esroGzsqg0Yw
TbYM9cwy70bjpNaJHnCsJ6dLjWFXe33lQE/DRvHfjcsz0AoG+gO4/pcsSxfz/xGD
vfsHu25GKdPTIt3LXd8teWc0EKtprOavb15RgHSLMX3H0TA5Sd7yrf3bAoGBAML7
t95O2uIRA/KqHbTJsg5yXznYBCYltB5zCoqfbWD7NlXLt2N1SN9GzC2KQ/FBDjgX
hX0GhKd/lfdrT5xoXhvRNbLEylsiOFAuCLKT4QuYwnCsSfAxqBPBt7EcRjtShHuM
BWEcR6p1I4rm3nz8OCjd5JF28SRRUZu3mJF1yHf1AoGBAMzv+kSGuxoYyMW+UZNL
NXTYL2ZwltjrGw+Yj1BYbIM1mUD7nslhQGA/+03D4TBtT60JrMVsJYo0qE+3wZoD
VgQy/wC2PYhk2S7Vdu5JoZVzuWfMQ+5f19DPRnt+iIhiqzQu9TyXmt3eB7Ly7VdZ
cweaop6KLC/RbitSoWxc2H6NAoGAeKqq+Ys3blw1Y0HgLHf+F260e+mxd6Ixu/5Z
8bwERtZwo9v03DpDJNp9lVlR/sXR9Oipldwo0DfnKT1iAVudkf2BC+c5i/i7fPNg
MpPN3A1nl68n+tvgNonWEmd7F09T6dmXoqeLunPP0TBbVGIKp0/bGiLPzTf+Fy4h
3zNTItkCgYEApNQkrCV/wqJ93BRmmwLxHS3lZmH/frnd3x7SBsTqJLtuD5CHDIl3
RUInBJ70DdjQF8g+TcqBIHt0laxjEx34reLIJRzwMhA0IGnkfXPKGJp3H6Ds4A33
aga5sjxqE91Y8gs1yy3QpbPt1HBxO1umLJ4HcnbfWroWRsqbuxHMnO0=
-----END RSA PRIVATE KEY-----
)EOF";

// Create global instances
ACCESSPOINT ap;  // Access Point object
myWIFI iWIFI;    // myWIFI object instance
internalDrivers iiDrivers;
myWIFI wifiObj;  // Another myWIFI object instance for general usage
WiFiClientSecure wifiClient = WiFiClientSecure();
PubSubClient client = PubSubClient();

// Constructor for myWIFI class (does nothing special here)
myWIFI::myWIFI() {}

// Function to setup WiFi connection
bool myWIFI::wiFiSetup(String ssid, String pass) {
  delay(10);             // Small delay for stability
  byte wiFiCounter = 0;  // Counter to retry WiFi connection attempts
  // Serial.println();
  // Serial.print("Connecting to ");
  // Serial.println(SSID);  // Print the SSID we're trying to connect to

  WiFi.begin(ssid.c_str(), pass.c_str());  // Start connecting to the WiFi network
  while (WiFi.status() != WL_CONNECTED) {  // Loop until connected
    digitalWrite(_wifiStatusLED, LOW);     // Turn LED ON
    delay(250);
    digitalWrite(_wifiStatusLED, HIGH);  // Turn LED OFF
    delay(250);
    // Serial.print(".");  // Print progress
    wiFiCounter++;  // Increment retry counter

    if (wiFiCounter >= 15) {  // If 15 tries failed, break out
      break;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {  // Still not connected after retries
    isWifiConnected = false;            // Set flag
    return false;                       // Return failure
  }

  // If connected successfully:
  // Serial.println();
  // Serial.println("WiFi connected");
  digitalWrite(_wifiStatusLED, HIGH);  // Keep LED ON
  delay(100);
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());  // Show assigned IP address
  ap.stopApServer();       // Stop the access point server
  ap.stopApWiFi();         // Stop access point WiFi
  isWifiConnected = true;  // Set flag
  return true;             // Return success
}


void myWIFI::mqttSetup(const char* MqttSever, int MqttPort) {
  // wifiClient.setInsecure();
  wifiClient.setCACert(root_ca);
  wifiClient.setCertificate(client_cert);
  wifiClient.setPrivateKey(client_key);
  if (wifiClient.connect(MqttSever, MqttPort)) {
    // Serial.println("Raw TLS socket connected!");
  } else {
    // Serial.println("Raw TLS socket connection failed!");
  }
  client.setClient(wifiClient);
  client.setServer(MqttSever, MqttPort);
  client.setCallback(MQTT_Pull);
}

// Check if WiFi is connected
bool myWIFI::wiFiLinkCheck() {
  return WiFi.status() == WL_CONNECTED;
}

// Reconnect to MQTT server, with Last Will Testament
void myWIFI::reconnectToMqtt(String pubTopic, String subTopic, String globalTopic) {
  String willMessage = "{" + deviceId + ":offline}";  // LWT (Last Will) message if device disconnects
  if (!client.connected()) {                          // If not connected to MQTT broker
    // Serial.print("Attempting MQTT connection...");
    // if (client.connect(deviceId.c_str(), mqttUserName, mqttUserPassword, pubTopic.c_str(), 1, true, willMessage.c_str())) {
    if (client.connect(deviceId.c_str(), pubTopic.c_str(), 1, true, willMessage.c_str())) {
      // Connected successfully
      // Serial.println("connected");
      subscribeTopics(subTopic);
      subscribeTopics(globalTopic);
      String onlineMessage = "{" + deviceId + ":online}";             // Publish online status
      client.publish(pubTopic.c_str(), onlineMessage.c_str(), true);  // Publish to MQTT
    } else {
      // Serial.println("Failed");
      mqttCounter++;                                                       // Increment failed MQTT connection counter
      if (mqttCounter >= 15) {                                             // After 15 failed tries
        ap.readSsidAndPasswordFromAP(apSSID.c_str(), apPassword.c_str());  // Start AP for new credentials
        isApStarted = true;                                                // Set AP started flag
        mqttCounter = 0;                                                   // Reset counter
      }
      // delay(100);  // Small delay before retry
    }
  }
}

// MQTT callback when a new message arrives
void myWIFI::MQTT_Pull(char* topic, byte* payload, unsigned int length) {
  String MqttRecdMsg = "";
  for (int i = 0; i < length; i++) {  // Convert payload to a clean String
    if (((char)payload[i] != ' ') && ((char)payload[i] != '\n')) {
      MqttRecdMsg += (char)payload[i];
    }
  }
  // Serial.println(MqttRecdMsg);
  // Serial.println();

  SplitData mqttDecodedMsg = wifiObj.splitStringByColon(MqttRecdMsg);  // Split message by ':'

  // Handle based on the message type
  if (mqttDecodedMsg.indexOneData == "wiFiCredentials") {
    SSID = mqttDecodedMsg.indexTwoData;        // New SSID received
    PASSWORD = mqttDecodedMsg.indexThreeData;  // New password received
    globallySSIDAndPasswordChange = true;      // Set flag to update credentials
  } else if (mqttDecodedMsg.indexOneData == "earasWiFiCredentialsFromEEPROM") {
    enterInAPMode = true;  // Enter AP mode to reset
  } else if (mqttDecodedMsg.indexOneData == "200") {
    responseOn200Request = true;  // Some external HTTP 200 OK response trigger
  } else if (mqttDecodedMsg.indexOneData == "publishInterval") {
    interval = mqttDecodedMsg.indexTwoData.toInt();                    // Update publishing interval
    wifiObj.writeOneByteInEEPROM(publishIntervalEEPROMAdd, interval);  // Save new interval
    alertMsg = "publishInterval is updated : " + String(interval);     // Store alert message
  } else if (mqttDecodedMsg.indexOneData == "autoMotionDetection") {
    if (mqttDecodedMsg.indexTwoData == "100") {
      autoMotionDetect_Flag = true;
      wifiObj.writeOneByteInEEPROM(autoMotionFlagEEPROMAddr, autoMotionDetect_Flag);
      alertMsg = "autoMotionActivate";
    } else if (mqttDecodedMsg.indexTwoData == "0") {
      autoMotionDetect_Flag = false;
      wifiObj.writeOneByteInEEPROM(autoMotionFlagEEPROMAddr, autoMotionDetect_Flag);
      alertMsg = "autoMotionDeactivate";
    }
  } else if (mqttDecodedMsg.indexOneData == "motionCounter") {
    motionCounter = (mqttDecodedMsg.indexTwoData).toInt();
    wifiObj.writeOneByteInEEPROM(motionCounterMsgEEPROM, motionCounter);
    alertMsg = "motionCounterUpdated: " + String(motionCounter);
  } else if (mqttDecodedMsg.indexOneData == "motionDetectThreshold") {
    motionDetectThreshold = (mqttDecodedMsg.indexTwoData).toInt();
    wifiObj.writeOneByteInEEPROM(motionDetectThresholdMsgEEPROM, motionDetectThreshold);
    alertMsg = "motionCounterUpdated: " + String(motionDetectThreshold);
  } else if (mqttDecodedMsg.indexOneData == "autoMotionStatus") {
    if (autoMotionDetect_Flag) {
      alertMsg = "autoMotion: Active";
    } else {
      alertMsg = "autoMotion: De-Active";
    }
  } else if (mqttDecodedMsg.indexOneData == "Relay1") {
    bool state = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.relayControl(_relayPins[0], state);
    relayloadsStatus[0] = state;
    wifiObj.writeOneByteInEEPROM(loadStateEEPROMAddress[0], state);
  } else if (mqttDecodedMsg.indexOneData == "Relay2") {
    bool state = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.relayControl(_relayPins[1], state);
    relayloadsStatus[1] = state;
    wifiObj.writeOneByteInEEPROM(loadStateEEPROMAddress[1], state);
  } else if (mqttDecodedMsg.indexOneData == "Relay3") {
    bool state = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.relayControl(_relayPins[2], state);
    relayloadsStatus[2] = state;
    wifiObj.writeOneByteInEEPROM(loadStateEEPROMAddress[2], state);
  } else if (mqttDecodedMsg.indexOneData == "Relay4") {
    bool state = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.relayControl(_relayPins[3], state);
    relayloadsStatus[3] = state;
    wifiObj.writeOneByteInEEPROM(loadStateEEPROMAddress[3], state);
  } else if (mqttDecodedMsg.indexOneData == "RelayAll") {
    for (int i = 0; i < 4; i++) {
      bool state = mqttDecodedMsg.indexTwoData.toInt();
      iiDrivers.relayControl(_relayPins[i], state);
      relayloadsStatus[i] = state;
      wifiObj.writeOneByteInEEPROM(loadStateEEPROMAddress[i], state);
      delay(1);
    }
  } else if (mqttDecodedMsg.indexOneData == "updateMqttTopic") {
    if (mqttDecodedMsg.indexThreeData.length() < 20) {
      if (mqttDecodedMsg.indexTwoData == "globalTopic") {
        wifiObj.unSubscribe(globalTopic + subTopic);
        globalTopic = mqttDecodedMsg.indexThreeData;
        wifiObj.subscribeTopics(globalTopic + subTopic);
        wifiObj.writeOneByteInEEPROM(globalTopicLenghtEEPROMAdd, globalTopic.length());
        wifiObj.storeStringInEEPROM(globalTopic, globalTopicEEPROMAdd);
        alertMsg = "GlobalTopicUpdated: " + globalTopic;
      }
    }
  } else if (mqttDecodedMsg.indexOneData == "storeStatus") {
    alertMsg = String(SSID) + ":" + String(PASSWORD) + ":" + String(globalTopic) + ":" + String(interval) + ":" + String(motionCounter) + ":" + String(motionDetectThreshold);
  } else {
    delay(1);  // Small delay for unknown message
  }
}

// Check if MQTT client is connected
bool myWIFI::CheckMQTTConnection() {
  return client.connected();
}

// Must be called regularly to maintain MQTT connection
void myWIFI::clientLoop() {
  client.loop();
}

// Publish a normal MQTT message
void myWIFI::publishMqttMsg(String pubTopic, String devID, float voltage, float current, double power, float voltage1, float current1, double power1, bool loadStatus1, bool loadStatus2, bool loadStatus3, bool loadStatus4) {
  String Final = "{device_id:" + devID + ":" + String(voltage) + ":" + String(current) + ":" + String(power) + ":" + String(voltage1) + ":" + String(current1) + ":" + String(power1) + ":" + String(loadStatus1) + ":" + String(loadStatus2) + ":" + String(loadStatus3) + ":" + String(loadStatus4) + "}";
  client.publish(pubTopic.c_str(), Final.c_str());
}

// Publish an alert MQTT message
void myWIFI::publishMqttMsg_Alert(String pubTopic, String devID, String Alert) {
  String Final = "{device_id:" + devID + ":" + Alert + "}";  // Format alert message
  client.publish(pubTopic.c_str(), Final.c_str());
}

// Create topic strings for publish/subscribe
subPubTopics myWIFI::createSubPubTopics(String devID, String SubTopic, String PubTopic, String globTopic) {
  subPubTopics Topics;
  Topics.Subscribe = devID + SubTopic;     // Device-specific subscription topic
  Topics.Publish = devID + PubTopic;       // Device-specific publish topic
  Topics.Global = globalTopic + SubTopic;  // Global subscription topic
  return Topics;
}

// Create a device ID from MAC address
String myWIFI::prepareDevID(byte mac[], String devPref) {
  char devID[30];
  snprintf(devID, sizeof(devID), "%s%02X%02X%02X", devPref.c_str(), mac[3], mac[4], mac[5]);
  return String(devID);  // Return device ID as string
}

// Clear stored WiFi credentials in EEPROM
void myWIFI::earasWiFiCredentialsFromEEPROM() {
  for (int i = 0; i < 42; i++) {
    EEPROM.write(i, 0);  // Clear first 42 bytes
  }
  EEPROM.commit();  // Save changes
}

// Write a single byte to EEPROM
void myWIFI::writeOneByteInEEPROM(int Add, byte data) {
  EEPROM.write(Add, data);  // Write at specific address
  EEPROM.commit();          // Save changes
}

// Store a full String in EEPROM
void myWIFI::storeStringInEEPROM(String data, byte Addr) {
  for (int i = 0; i < data.length(); i++) {
    EEPROM.write(Addr + i, data.charAt(i));  // Write each character
  }
  EEPROM.commit();  // Save changes
}

// Load a string from EEPROM
String myWIFI::loadStringFromEEPROM(byte Addr, byte Length) {
  String readData = "";
  for (int i = Addr; i < (Addr + Length); i++) {
    readData += char(EEPROM.read(i));  // Read each character
  }
  return readData;
}

void myWIFI::subscribeTopics(String subsTopic) {
  client.subscribe(subsTopic.c_str());
}

void myWIFI::unSubscribe(String subPubTopic) {
  client.unsubscribe(subPubTopic.c_str());
}

// Split a string by ':' delimiter into parts
SplitData myWIFI::splitStringByColon(const String& data) {
  SplitData mqttMsg;
  int firstIndex = data.indexOf(':');
  if (firstIndex != -1) {  // Found first delimiter
    mqttMsg.indexOneData = data.substring(0, firstIndex);
    int secondIndex = data.indexOf(':', firstIndex + 1);
    if (secondIndex != -1) {  // Found second delimiter
      mqttMsg.indexTwoData = data.substring(firstIndex + 1, secondIndex);
      mqttMsg.indexThreeData = data.substring(secondIndex + 1);
      if (mqttMsg.indexThreeData.length() > 0) {
        // third index exists
      }
    } else {
      mqttMsg.indexTwoData = data.substring(firstIndex + 1);  // Only two parts
    }
  } else {
    mqttMsg.indexOneData = data.substring(firstIndex + 1);  // No delimiter found, strange fallback
  }
  return mqttMsg;
}
