
#include "Config.h"
#include "internalDrivers.h"
#include "myWIFI.h"
#include "accessPoint.h"


ACCESSPOINT AP;
myWIFI wiFI;
internalDrivers iDrivers;
subPubTopics Topic;

void setup() {
  delay(5000);
  Serial.begin(115200);
  iDrivers.gpioInit();
  iDrivers.readDataFromEEPROM();
  wiFI.wiFiSetup(SSID, PASSWORD);
  apSSID = wiFI.prepareDevID(MAC, apSSID);
  deviceId = wiFI.prepareDevID(MAC, devNamePrefix);
  AP.accessPointSetup();
  wiFI.mqttSetup(ServerMQTT, MqttPort);
  Topic = wiFI.createSubPubTopics(deviceId, subTopic, pubTopic, globalTopic);  // establish the connections with Mqtt
  wiFI.reconnectToMqtt(Topic.Publish, Topic.Subscribe, Topic.Global);
  wiFI.creatingTopics();
}

void loop() {
  wiFI.clientLoop();

  iDrivers.requestDataFromSlave();

  AP.updateTheSSIDAndPASSFromMqttIfAvailable();
  if (enterInAPMode) {
    mqttCounter = 0;
    wiFI.earasWiFiCredentialsFromEEPROM();
    SSID = "";
    PASSWORD = "";
    AP.accessPointSetup();
    enterInAPMode = false;
  }
  delay(1000);
}
