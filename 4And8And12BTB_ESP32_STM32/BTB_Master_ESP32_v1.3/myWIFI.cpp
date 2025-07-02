#include "myWiFi.h"
#include "Config.h"
#include "accessPoint.h"
#include "internalDrivers.h"
#include <EEPROM.h>

/* // Certificates and key
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC6TCCAdGgAwIBAgIUL+Qw3ZBWWW2KuZGeTmlHHYncy0EwDQYJKoZIhvcNAQEL
BQAwFjEUMBIGA1UEAwwLSU9ULVJPT1QtQ0EwHhcNMjQxMTI5MTM0NDQxWhcNMzQx
MTI3MTM0NDQxWjAWMRQwEgYDVQQDDAtJT1QtUk9PVC1DQTCCASIwDQYJKoZIhvcN
AQEBBQADggEPADCCAQoCggEBAI+8Bk4EgZDUOGFa056SbjQ/SFWdSlglVWUSC6sk
xCjO0NBuciVsMwOOFo+M9F2tIEo74DMdJwp4Ggs8w2Zw0NOYioI4VyKry55buK2Y
h4149wuRt3QayIBqy4tTWw+iFx9v+otdmudfRNWsCmrUAG+maXB3mlpaJauUav4J
DcRtX2ex/vWT6woVxf6GnwE9mZRgVrKHMNHVVe0mDAdDyFyRhFou13lPUSBBDzdK
H/3tszh/euHHQEcjGOjbv6S1uLC9WbTJqKqEO4z5EzNhw6Co44xOAjZo25L+QYtw
0Y5UJk1QHlsY2VjgiuoUf3VMK+RXVsWdB5kEwu3ivB+K5j0CAwEAAaMvMC0wDAYD
VR0TBAUwAwEB/zAdBgNVHQ4EFgQUA/CYVEtdNDD3z96n74ByYLb2dDQwDQYJKoZI
hvcNAQELBQADggEBAElI37iWwKUXHyVbKN9Te50Ixz7WLgDnRSDQie9g4vQvbsXt
p+a9uQde8FAIn4+fGnXi073CaVqFIS4x70ZlW96IvVLUmik0OltTjyRQnC7FqnSV
vppToWfOmVutn/dDXlaxKIpsiHTkmQ+IOJjlazflEhsWOcocxidYfuHik8MlAHbc
q4TK+AYK+ESsP60CScsqDxlfeBHJTU2E1BN599pupe4BJUVt88SomfHlqyX4xkR8
EaNnB+OJTGHhReLTcfRWsINrQPNdmr+3NvARHqQoAmWCYf/F+iBCIBFTHfSR4o+3
K129Kynhmg28ua05Nd63Siqx1eFoYfC080Vu9dc=
-----END CERTIFICATE-----
)EOF";

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
)EOF"; */

ACCESSPOINT ap;
myWIFI iWIFI;
internalDrivers iiDrivers;
WiFiClient WLS_Client;
PubSubClient client(WLS_Client);
subPubTopics wTopic;
// BearSSL::WiFiClientSecure secureClient;

myWIFI::myWIFI() {}

bool myWIFI::wiFiSetup(String ssid, String pass) {
  delay(10);
  byte wiFiCounter = 0;
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(ssid.c_str(), pass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(_wifiStatusLED, HIGH);
    delay(250);
    digitalWrite(_wifiStatusLED, LOW);
    delay(250);
    Serial.print(".");
    wiFiCounter++;
    if (wiFiCounter >= 15) {
      // isWifiConnected = false;
      // return false;
      break;
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    isWifiConnected = false;
    return false;
  }

  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(_wifiStatusLED, HIGH);
  delay(100);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  ap.stopApServer();
  ap.stopApWiFi();
  isWifiConnected = true;
  return true;
}

//For TCP
void myWIFI::mqttSetup(const char* MqttSever, int MqttPort) {
  client.setServer(MqttSever, MqttPort);
  client.setCallback(MQTT_Pull);
}

// //For SSL
// void WIFI::mqttSetup(const char* MqttSever, int MqttPort) {
//   // Configure secure connection
//   secureClient.setInsecure();
//   // secureClient.setTrustAnchors(new BearSSL::X509List(ca_cert));
//   secureClient.setClientRSACert(
//     new BearSSL::X509List(client_cert),
//     new BearSSL::PrivateKey(client_key));
//   client.setServer(MqttSever, MqttPort);
//   client.setCallback(MQTT_Pull);
// }

bool myWIFI::wiFiLinkCheck() {
  return WiFi.status() == WL_CONNECTED;
}

void myWIFI::reconnectToMqtt(String pubTopic, String subTopic, String globalTopic) {
  String willMessage = "{" + deviceId + ":offline}";  // Last Will and Testament message
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(deviceId.c_str(), mqttUserName, mqttUserPassword, pubTopic.c_str(), 1, true, willMessage.c_str())) {
      // if (client.connect(deviceId.c_str(), pubTopic.c_str(), 1, true, willMessage.c_str())) {
      Serial.println("connected");
      client.subscribe(subTopic.c_str());
      client.subscribe(globalTopic.c_str());
      String onlineMessage = "{" + deviceId + ":online}";
      client.publish(pubTopic.c_str(), onlineMessage.c_str(), true);
    } else {
      mqttCounter++;
      if (mqttCounter >= 15) {
        ap.readSsidAndPasswordFromAP(apSSID.c_str(), apPassword.c_str());  // Read credentials from AP
        isApStarted = true;
        mqttCounter = 0;
      }
      delay(100);
    }
  }
}

void myWIFI::MQTT_Pull(char* topic, byte* payload, unsigned int length) {
  String MqttRecdMsg = "";
  for (int i = 0; i < length; i++) {
    if (((char)payload[i] != ' ') && ((char)payload[i] != '\n')) {
      MqttRecdMsg += (char)payload[i];
    }
  }
  SplitData mqttDecodedMsg = iiDrivers.splitStringByColon(MqttRecdMsg);
  if (mqttDecodedMsg.indexOneData == "wiFiCredentials") {
    SSID = mqttDecodedMsg.indexTwoData;        // Get SSID
    PASSWORD = mqttDecodedMsg.indexThreeData;  // Get Password
    globallySSIDAndPasswordChange = true;
  } else if (mqttDecodedMsg.indexOneData == "earasWiFiCredentialsFromEEPROM") {
    enterInAPMode = true;
  } else if (mqttDecodedMsg.indexOneData == "200") {
    responseOn200Request = true;
  } else if (mqttDecodedMsg.indexOneData == "publishInterval") {
    interval = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.writeOneByteInEEPROM(publishIntervalEEPROMAdd, interval);
    alertMsg = "publishInterval is updated : " + String(interval);
  } else if (mqttDecodedMsg.indexOneData == "i2cScan") {
    iiDrivers.i2cScanner();
  } else if (mqttDecodedMsg.indexOneData == "restart") {
    ESP.restart();
  } else if (mqttDecodedMsg.indexOneData == "slaveRestart") {
    iiDrivers.sendDataToSlave(STM32_ADDRESS, "slaveRestart");
    iiDrivers.sendDataToSlave(STM32_ADDRESS1, "slaveRestart");
    iiDrivers.sendDataToSlave(STM32_ADDRESS2, "slaveRestart");
  } else if (mqttDecodedMsg.indexOneData == "Relay1" || mqttDecodedMsg.indexOneData == "Relay2" || mqttDecodedMsg.indexOneData == "Relay3" || mqttDecodedMsg.indexOneData == "Relay4") {
    if (mqttDecodedMsg.indexOneData == "Relay1") {
      String relay = "1";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS, msg);
    } else if (mqttDecodedMsg.indexOneData == "Relay2") {
      String relay = "2";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS, msg);
    } else if (mqttDecodedMsg.indexOneData == "Relay3") {
      String relay = "3";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS, msg);
    } else if (mqttDecodedMsg.indexOneData == "Relay4") {
      String relay = "4";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS, msg);
    } else {
      delay(1);
    }
  } else if (mqttDecodedMsg.indexOneData == "Relay5" || mqttDecodedMsg.indexOneData == "Relay6" || mqttDecodedMsg.indexOneData == "Relay7" || mqttDecodedMsg.indexOneData == "Relay8") {
    if (mqttDecodedMsg.indexOneData == "Relay5") {
      String relay = "1";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS1, msg);
    } else if (mqttDecodedMsg.indexOneData == "Relay6") {
      String relay = "2";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS1, msg);
    } else if (mqttDecodedMsg.indexOneData == "Relay7") {
      String relay = "3";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS1, msg);
    } else if (mqttDecodedMsg.indexOneData == "Relay8") {
      String relay = "4";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS1, msg);
    } else {
      delay(1);
    }
  } else if (mqttDecodedMsg.indexOneData == "Relay9" || mqttDecodedMsg.indexOneData == "Relay10" || mqttDecodedMsg.indexOneData == "Relay11" || mqttDecodedMsg.indexOneData == "Relay12")
    if (mqttDecodedMsg.indexOneData == "Relay9") {
      String relay = "1";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS2, msg);
    } else if (mqttDecodedMsg.indexOneData == "Relay10") {
      String relay = "2";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS2, msg);
    } else if (mqttDecodedMsg.indexOneData == "Relay11") {
      String relay = "3";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS2, msg);
    } else if (mqttDecodedMsg.indexOneData == "Relay12") {
      String relay = "4";
      String msg = relay + ":" + mqttDecodedMsg.indexTwoData;
      iiDrivers.sendDataToSlave(STM32_ADDRESS2, msg);
    } else {
      delay(1);
    }
  else {
    delay(1);
  }
}


void myWIFI::checkMqttConnectionAndPublishData(String PublishingData) {
  /* One Mints Millis loop */
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= (interval * 1000) || responseOn200Request || !isWifiConnected || alertMsg != "") {
    if (currentTime - previousTime >= (interval * 1000)) {  // RESTART TO MEASURE ONE MINUTES INTERVAL IF ONE MINUTES DONE FROM LAST RESET
      previousTime = currentTime;
    }
    if (wiFiLinkCheck()) {
      isWifiConnected = true;
      digitalWrite(_wifiStatusLED, HIGH);
      if (isApStarted) {
        wiFiRetryCounter = 0;
        isApStarted = false;
        ap.stopApServer();
        ap.stopApWiFi();
      }
      if (CheckMQTTConnection()) {
        if (alertMsg != "") {
          publishMqttMsg_Alert(wTopic.Publish, deviceId, alertMsg);
          alertMsg = "";
        }
        if (responseOn200Request) {
          publishMqttMsg(wTopic.Publish, "200", PublishingData);
          responseOn200Request = false;
        } else {
          publishMqttMsg(wTopic.Publish, deviceId, PublishingData);
          //previousTime = currentTime;
        }
      } else {
        if (!CheckMQTTConnection()) {
          reconnectToMqtt(wTopic.Publish, wTopic.Subscribe, wTopic.Global);
        } else {
          delay(100);
        }
      }
    } else {
      wiFiSetup(SSID, PASSWORD);
      wiFiRetryCounter += 1;
      if (wiFiRetryCounter >= 2 && (!isApStarted)) {
        ap.readSsidAndPasswordFromAP(apSSID.c_str(), apPassword.c_str());
        isApStarted = true;
      }
    }
  }
}

bool myWIFI::CheckMQTTConnection() {
  return client.connected();
}

void myWIFI::clientLoop() {
  client.loop();
}

void myWIFI::publishMqttMsg(String pubTopic, String devID, String Msg) {
  String Final = "{device_id:" + devID + ":" + Msg + "}";
  client.publish(pubTopic.c_str(), Final.c_str());
}

void myWIFI::publishMqttMsg_Alert(String pubTopic, String devID, String Alert) {
  String Final = "{device_id:" + devID + ":" + Alert + "}";
  client.publish(pubTopic.c_str(), Final.c_str());
}

subPubTopics myWIFI::createSubPubTopics(String devID, String SubTopic, String PubTopic, String globTopic) {
  subPubTopics Topics;
  Topics.Subscribe = devID + SubTopic;
  Topics.Publish = devID + PubTopic;
  Topics.Global = globalTopic + SubTopic;
  return Topics;
}

String myWIFI::prepareDevID(byte mac[], String devPref) {
  char devID[30];
  snprintf(devID, sizeof(devID), "%s%02X%02X%02X", devPref.c_str(), mac[3], mac[4], mac[5]);
  return String(devID);
}

void myWIFI::creatingTopics() {
  wTopic = createSubPubTopics(deviceId, subTopic, pubTopic, globalTopic);
}


void myWIFI::earasWiFiCredentialsFromEEPROM() {
  Serial.println("Clearing EEORM");
  for (int i = 0; i < 42; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}