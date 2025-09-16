/*
   Name Of Project: Smart Plug Over WiFi
   MCU used: ESP8266 SINGLE CORE
   Functions: Load ON/OFF over MQTT and Power measurement
   Programmer: Nandakishor Dhote
   Author: EVOLUZN INDIA PRIVATE LIMITED
   Firmware Status: 1) Power data reading from CSE IC
                    2) Publishing the data over MQTT on one mints frequency
                    3) reading wifi creadentials from access point
                    4) MQTT secure with CA Certifications 
*/

#include "Config.h"
#include "internalDrivers.h"
#include "myWIFI.h"
#include "CSE7759.h"
#include "CSE7759_two.h"
#include "accessPoint.h"

ACCESSPOINT AP;
CSE7759 cse7759;
CSE7759_two cse7759_two;
myWIFI wiFI;
internalDrivers iDrivers;
subPubTopics Topic;
PowerData powerData;
PowerData2 powerData2;

void setup() {
  Serial.begin(9600);                       // for debugging
  Serial1.begin(4800, SERIAL_8N1, 16, -1);  // Start UART1 at 4800 baud, format 8N1, RX on GPIO16, TX disabled
  Serial2.begin(4800, SERIAL_8N1, 27, -1);  // Start UART2 at 4800 baud, format 8N1, RX on GPIO27, TX disabled
  iDrivers.gpioInit();
  iDrivers.readDataFromEEPROM();
  wiFI.wiFiSetup(SSID, PASSWORD);
  apSSID = wiFI.prepareDevID(MAC, apSSID);
  deviceId = wiFI.prepareDevID(MAC, devNamePrefix);
  AP.accessPointSetup();
  wiFI.mqttSetup(ServerMQTT, MqttPort);
  Topic = wiFI.createSubPubTopics(deviceId, subTopic, pubTopic, globalTopic);  // establish the connections with Mqtt
  wiFI.reconnectToMqtt(Topic.Publish, Topic.Subscribe, Topic.Global);
  cse7759.initialize_power_data(&powerData);
  cse7759_two.initialize_power_data_two(&powerData2);
}

void loop() {
  wiFI.clientLoop();

  iDrivers.motionDetectionUsingPIR();
  powerData = cse7759.validateTheCSEChipData(cse7759.read_power_data());
  powerData2 = cse7759_two.validateTheCSEChipData_two(cse7759_two.read_power_data_two());

  AP.updateTheSSIDAndPASSFromMqttIfAvailable();
  if (enterInAPMode) {
    mqttCounter = 0;
    wiFI.earasWiFiCredentialsFromEEPROM();
    SSID = "";
    PASSWORD = "";
    AP.accessPointSetup();
    enterInAPMode = false;
  }
  /* One Mints Millis loop */
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= (interval * 1000) || responseOn200Request || !isWifiConnected || alertMsg != "") {
    if (wiFI.wiFiLinkCheck()) {
      isWifiConnected = true;
      digitalWrite(_wifiStatusLED, HIGH);
      if (isApStarted) {
        wiFiRetryCounter = 0;
        isApStarted = false;
        AP.stopApServer();
        AP.stopApWiFi();
      }
      if (wiFI.CheckMQTTConnection()) {
        if (alertMsg != "") {
          wiFI.publishMqttMsg_Alert(Topic.Publish, deviceId, alertMsg);
          alertMsg = "";
        }
        if ((currentTime - previousTime >= (interval * 1000)) || responseOn200Request) {
          if (responseOn200Request) {
            wiFI.publishMqttMsg(Topic.Publish, "200", powerData.voltage, powerData.current, powerData.power, powerData2.voltage, powerData2.current, powerData2.power, relayloadsStatus[0], relayloadsStatus[1], relayloadsStatus[2], relayloadsStatus[3]);
            responseOn200Request = false;
          } else {
            wiFI.publishMqttMsg(Topic.Publish, deviceId, powerData.voltage, powerData.current, powerData.power, powerData2.voltage, powerData2.current, powerData2.power, relayloadsStatus[0], relayloadsStatus[1], relayloadsStatus[2], relayloadsStatus[3]);
            previousTime = currentTime;
          }
        }
      } else {
        if (!wiFI.CheckMQTTConnection()) {
          wiFI.reconnectToMqtt(Topic.Publish, Topic.Subscribe, Topic.Global);
        } else {
          delay(100);
        }
      }
    } else {
      wiFI.wiFiSetup(SSID, PASSWORD);
      wiFiRetryCounter += 1;
      if (wiFiRetryCounter >= 2 && (!isApStarted)) {
        AP.readSsidAndPasswordFromAP(apSSID.c_str(), apPassword.c_str());
        isApStarted = true;
      }
    }
  }
  delay(2000);
}
