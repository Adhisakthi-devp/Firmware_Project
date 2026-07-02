#include <WebServer.h>
WebServer server(80);
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <RTClib.h>
#include<WebServer.h>

RTC_DS3231 rtc;
#include "config.h"
#include "EEPROM_Init.h"

#include "WiFi_MQTT.h"

#define txModBus 15
#define rxModBus 4
#define enModBus 2 

ModbusMaster node;

void Modbus_Init(){
  Serial.println("Modbus Initialisation");
  Serial1.begin(9600, SERIAL_8N1, rxModBus,txModBus); 
  pinMode(enModBus , OUTPUT);
  digitalWrite(enModBus , LOW);
  node.begin(MB_Slave_ID, Serial1);
  node.preTransmission(modbus_pre);
  node.postTransmission(modbus_post);
}

String getD(){
  Serial.print("Command : GET_DATE");
  DateTime now=rtc.now();
  char DATE[20];
  sprintf(DATE,"%02d/%02d/%04d",now.day(),now.month(),now.year());
  return String(DATE);
}
String getT(){
  Serial.print("Command : GET_TIME");
  DateTime now=rtc.now();
  char TIME[25];
  sprintf(TIME,"%02d:%02d:%02d",now.hour(),now.minute(),now.second());
  return String(TIME);
  
}
void RTC_setup(){
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("RTC Finding Failed!");
  }
  else {
    if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }  
}
void modbus_pre()  { digitalWrite(enModBus , HIGH); }
void modbus_post() { digitalWrite(enModBus , LOW); }
void startAP(){
    WiFi.mode(WIFI_AP);

    WiFi.softAP(
        "IoT_Gateway_Config",
        "12345678"
    );

    Serial.println("AP Started");
    Serial.print("IP Address : ");
    Serial.println(WiFi.softAPIP());
}
void setup(){
    Serial.begin(115200);
    delay(1000);
    loadConfig();
    printConfig();
    RTC_setup();
    Modbus_Init();
    connectWiFi();      
    connectMQTT();
    create_json();
}
void loop() {
checkWiFi();
mqtt.loop();  
  static unsigned long last = 0;
  if (millis() - last > INT) {
    last = millis();    
    create_json();     
    mqtt.publish(Pub_Topic_1.c_str(), json.c_str());
  }
}
