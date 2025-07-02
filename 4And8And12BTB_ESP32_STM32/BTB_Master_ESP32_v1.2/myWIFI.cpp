#include "myWiFi.h"
#include "Config.h"
#include "accessPoint.h"
#include "internalDrivers.h"
#include <EEPROM.h>

ACCESSPOINT ap;
myWIFI iWIFI;
internalDrivers iiDrivers;
WiFiClient WLS_Client;
PubSubClient client(WLS_Client);
subPubTopics wTopic;

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

void myWIFI::mqttSetup(const char* MqttSever, int MqttPort) {
  client.setServer(MqttSever, MqttPort);
  client.setCallback(MQTT_Pull);
}

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
  } else {
    delay(1);
  }
}

void myWIFI::checkMqttConnectionAndPublishData(String PublishingData) {
  /* One Mints Millis loop */
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= (interval * 1000) || responseOn200Request || !isWifiConnected) {
    if (currentTime - previousTime >= (interval * 1000)) {  // RESTART TO MEASURE ONE MINUTES INTERVAL IF ONE MINUTES DONE FROM LAST RESET
      previousTime = currentTime;
    }
    if (iWIFI.wiFiLinkCheck()) {
      isWifiConnected = true;
      digitalWrite(_wifiStatusLED, HIGH);
      if (isApStarted) {
        wiFiRetryCounter = 0;
        isApStarted = false;
        ap.stopApServer();
        ap.stopApWiFi();
      }
      if (iWIFI.CheckMQTTConnection()) {
        if ((currentTime - previousTime >= (interval * 1000)) || responseOn200Request) {
          if (responseOn200Request) {
            iWIFI.publishMqttMsg(wTopic.Publish, "200", PublishingData);
            responseOn200Request = false;
          } else {
            iWIFI.publishMqttMsg(wTopic.Publish, deviceId, PublishingData);
            //previousTime = currentTime;
          }
        }
      } else {
        if (!iWIFI.CheckMQTTConnection()) {
          iWIFI.reconnectToMqtt(wTopic.Publish, wTopic.Subscribe, wTopic.Global);
        } else {
          delay(100);
        }
      }
    } else {
      iWIFI.wiFiSetup(SSID, PASSWORD);
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
  wTopic = createSubPubTopics(deviceId, subTopic, pubTopic, globalTopic);
  snprintf(devID, sizeof(devID), "%s%02X%02X%02X", devPref.c_str(), mac[3], mac[4], mac[5]);
  return String(devID);
}


void myWIFI::earasWiFiCredentialsFromEEPROM() {
  Serial.println("Clearing EEORM");
  for (int i = 0; i < 42; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}