#include "wiFi.h"
#include "Config.h"
#include "String.h"
#include "internalDrivers.h"
#include "accessPoint.h"

ACCESSPOINT ap;
WIFI iWIFI;
internalDrivers iiDrivers;
WiFiClient WLS_Client;
PubSubClient client(WLS_Client);

WIFI::WIFI() {
}

bool WIFI::wiFiSetup(String ssid, String pass) {
  delay(10);
  byte wiFiCounter = 0;
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(ssid.c_str(), pass.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(_statusLED, HIGH);
    delay(250);
    digitalWrite(_statusLED, LOW);
    delay(250);
    Serial.print(".");
    wiFiCounter++;
    if (wiFiCounter >= 15) {
      return false;
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(_statusLED, LOW);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  ap.stopApServer();
  ap.stopApWiFi();
  return true;
}

void WIFI::mqttSetup(const char* MqttSever, int MqttPort) {
  client.setServer(MqttSever, MqttPort);
  client.setCallback(MQTT_Pull);
}

bool WIFI::wiFiLinkCheck() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  return true;
}

void WIFI::reconnectToMqtt(String pubTopic, String subTopic, String globalTopic) {
  String willMessage = "{" + deviceId + ":offline}";  // Last Will and Testament message

  if (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    //    if (client.connect(prepareDevID(MAC, devNamePrefix).c_str(), mqttUserName, mqttUserPassword)) {
    // if (client.connect(prepareDevID(MAC, devNamePrefix).c_str()))
    if (client.connect(deviceId.c_str(), pubTopic.c_str(), 1, true, willMessage.c_str())) {
      //Serial.println("connected");
      client.subscribe(subTopic.c_str());
      client.subscribe(globalTopic.c_str());

      String onlineMessage = "{" + deviceId + ":online}";
      client.publish(pubTopic.c_str(), onlineMessage.c_str(), true);
    } else {
      mqttCounter++;
      if (mqttCounter >= 15) {
        //enterInAPMode = true;
        ap.readSsidAndPasswordFromAP(apSSID.c_str(), apPassword.c_str());
        isApStarted = true;
      }
      delay(100);
    }
  }
}

void WIFI::MQTT_Pull(char* topic, byte* payload, unsigned int length) {
  String MqttRecdMsg = "";
  for (int i = 0; i < length; i++) {
    MqttRecdMsg += (char)payload[i];
  }
  Serial.println(MqttRecdMsg);
  Serial.println();

  SplitData mqttDecodedMsg = iiDrivers.splitStringByColon(MqttRecdMsg);

  if (MqttRecdMsg == "200") {
    responseOn200Request = true;
  } else if (mqttDecodedMsg.indexOneData == "Relay1") {
    relayloadsStatus[0] = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.relayControl(_relayPins[0], relayloadsStatus[0]);
    iiDrivers.writeOneByteInEEPROM(loadStateEEPROMAddress[0], relayloadsStatus[0]);
  } else if (mqttDecodedMsg.indexOneData == "Relay2") {
    relayloadsStatus[1] = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.relayControl(_relayPins[1], relayloadsStatus[1]);
    iiDrivers.writeOneByteInEEPROM(loadStateEEPROMAddress[1], relayloadsStatus[1]);
  } else if (mqttDecodedMsg.indexOneData == "Relay3") {
    relayloadsStatus[2] = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.relayControl(_relayPins[2], relayloadsStatus[2]);
    iiDrivers.writeOneByteInEEPROM(loadStateEEPROMAddress[2], relayloadsStatus[2]);
  } else if (mqttDecodedMsg.indexOneData == "Relay4") {
    relayloadsStatus[3] = mqttDecodedMsg.indexTwoData.toInt();
    iiDrivers.relayControl(_relayPins[3], relayloadsStatus[3]);
    iiDrivers.writeOneByteInEEPROM(loadStateEEPROMAddress[3], relayloadsStatus[3]);
  } else if (mqttDecodedMsg.indexOneData == "wiFiCredentials") {
    SSID = mqttDecodedMsg.indexTwoData;        // Get SSID
    PASSWORD = mqttDecodedMsg.indexThreeData;  // Get Password
    globallySSIDAndPasswordChange = true;
  } else if (MqttRecdMsg == "earasWiFiCredentialsFromEEPROM") {
    enterInAPMode = true;
  } else {
  }
}

bool WIFI::CheckMQTTConnection() {
  if (client.connected()) {
    return true;
  }
  return false;
}

void WIFI::clientLoop() {
  client.loop();
}

void WIFI::publishMqttMsg(String pubTopic, String devID, float voltage, float current, float power, bool loadOneStatus, bool loadTwoStatus, bool loadThreeStatus, bool loadFourStatus) {
  String Final = "{device_id:" + devID + ":" + String(voltage) + ":" + String(current) + ":" + String(power) + ":" + String(loadOneStatus) + ":" + String(loadTwoStatus) + ":" + String(loadThreeStatus) + ":" + String(loadFourStatus) + "}";
  client.publish(pubTopic.c_str(), Final.c_str());
}

void WIFI::publishSingleMqttMsg(String pubTopic, String Msg) {
  client.publish(pubTopic.c_str(), Msg.c_str());
}

subPubTopics WIFI::createSubPubTopics(String devID, String SubTopic, String PubTopic, String globTopic) {
  subPubTopics Topics;
  Topics.Subscribe = devID + SubTopic;
  Topics.Publish = devID + PubTopic;
  Topics.Global = globalTopic + SubTopic;
  return Topics;
}

String WIFI::prepareDevID(byte mac[], String devPref) {
  char devID[30];
  snprintf(devID, sizeof(devID), "%s%02X%02X%02X", devPref.c_str(), mac[3], mac[4], mac[5]);
  return String(devID);
}
