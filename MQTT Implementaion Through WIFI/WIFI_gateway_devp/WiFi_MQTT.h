#pragma once
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "config.h"
#include "EEPROM_Init.h"
#include <RTClib.h>

extern RTC_DS3231 rtc;

WiFiClient gsmClient;
PubSubClient mqtt(gsmClient);

unsigned long INT = 5000;

void connectWiFi();
void connectMQTT();
void checkWiFi();
void message(char *topic, byte *payload, unsigned int length);
void publishConfig();
void updateConfig(String json);

void connectWiFi(){
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("IoT_Gateway_Config","12345678");
    WiFi.begin(WIFI_SSID.c_str(),WIFI_PASSWORD.c_str());
    Serial.print("Connecting WiFi");
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi Connected");
    Serial.print("STA IP : ");
    Serial.println(WiFi.localIP());
    Serial.print("AP IP : ");
    Serial.println(WiFi.softAPIP());
    mqtt.setServer(Server_Address.c_str(), Port);
    mqtt.setCallback(message);
}
void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.println("Connecting MQTT...");
    delay(5000);
    if (mqtt.connect(MQTT_ClientId.c_str(), MQTT_User.c_str(), MQTT_Password.c_str()))
    {
      Serial.println("MQTT Connected OK");
      mqtt.subscribe(Sub_Topic.c_str());
      mqtt.subscribe("M2627001_CONFIG");
    } else {
      Serial.printf("MQTT Link Failed, rc=%d\n", mqtt.state());
      delay(5000);
    }
  }
}
void checkWiFi(){
    
    int connectedStations = WiFi.softAPgetStationNum();
    if (!mqtt.connected()){
        connectMQTT();
    }
}
void updateConfig(String JsonMess){
    mqtt.publish(Pub_Topic_2.c_str(), "CONFIG_UPDATING");
    DynamicJsonDocument doc(512);
    DeserializationError err = deserializeJson(doc, JsonMess);
    if (err){
        Serial.println("JSON Parse Failed");
        mqtt.publish(Pub_Topic_2.c_str(), "JSON_PARSE_FAILED");
        return;
    }
    if (doc["Device_ID"]){
        setDeviceID(doc["Device_ID"].as<String>());
    }
    if (doc["Server_Address"]){
        String server = doc["Server_Address"].as<String>();
        uint16_t port = doc["Port"] | Port;
        setServerAdd(server, port);
    }
    if (doc["APN"]){
        setAPN(doc["APN"].as<String>());
    }
    if (doc["MQTT_User"]){
        MQTT_User = doc["MQTT_User"].as<String>();
        EEPROM_WriteStr(ADDR_MQTT_User, MQTT_User);
    }
    if (doc["MQTT_Password"]){
        MQTT_Password = doc["MQTT_Password"].as<String>();
        EEPROM_WriteStr(ADDR_MQTT_Password, MQTT_Password);
    }
    if (doc["MQTT_ClientId"]){
        MQTT_ClientId = doc["MQTT_ClientId"].as<String>();
        EEPROM_WriteStr(ADDR_MQTT_ClientId, MQTT_ClientId);
    }
    if (doc["Pub_Topic_1"]){
        Pub_Topic_1 = doc["Pub_Topic_1"].as<String>();
        EEPROM_WriteStr(ADDR_Pub_TOPIC_1, Pub_Topic_1);
    }
    if (doc["Pub_Topic_2"])    {
        Pub_Topic_2 = doc["Pub_Topic_2"].as<String>();
        EEPROM_WriteStr(ADDR_Pub_TOPIC_2, Pub_Topic_2);
    }
    if (doc["Sub_Topic"]){
        Sub_Topic = doc["Sub_Topic"].as<String>();
        EEPROM_WriteStr(ADDR_Sub_TOPIC, Sub_Topic);
    }
     if (doc["Ping_Interval"]){
        Ping_Int = doc["Ping_Interval"];
        EEPROM_WriteInt(ADDR_PING_INTERVAL, Ping_Int);

        INT = Ping_Int * 1000UL;
    }
    if (doc["MB_Slave_ID"]){
        MB_Slave_ID=doc["MB_Slave_ID"];
        EEPROM_WriteInt(ADDR_MB_SLAVE_ID, MB_Slave_ID);
    }
    if (doc["MB_Address"]){
        MB_Address = doc["MB_Address"];
        EEPROM_WriteInt(ADDR_MB_ADDRESS, MB_Address);
    }
    mqtt.publish(Pub_Topic_2.c_str(),"CONFIG_UPDATED");
    mqtt.publish(Pub_Topic_2.c_str(),"RESTART_REQUIRED");
    Serial.println("Configuration Updated Successfully");
}
void publishConfig(){
    DynamicJsonDocument doc(512);
    doc["Device_ID"] = Device_ID;
    doc["Server_Address"] = Server_Address;
    doc["Port"] = Port;
    doc["APN"] = APN;
    doc["Ping_Interval"] = Ping_Int;

    doc["MB_Slave_ID"] = MB_Slave_ID;
    doc["MB_Address"] = MB_Address;

    doc["Pub_Topic_1"] = Pub_Topic_1;
    doc["Pub_Topic_2"] = Pub_Topic_2;

    doc["Sub_Topic"] = Sub_Topic;

    String details;
    serializeJsonPretty(doc, details);
    mqtt.publish(Pub_Topic_2.c_str(), details.c_str());

    Serial.println("Config Published:");
    Serial.println(details);
}
void message(char *topic, byte *payload, unsigned int length){
    String cmd = "";
    for (  int i = 0; i < length; i++) { 
        cmd += (char)payload[i];
    }
    cmd.trim();
    Serial.println("Received Command : " + cmd);
    
    if (cmd == "GET_DATE"){
        String DATE = getD();
        mqtt.publish(Pub_Topic_2.c_str(), DATE.c_str());
        Serial.println("Published Date : " + DATE);
    }
    else if (cmd == "GET_TIME"){
        String TIME = getT();
        mqtt.publish(Pub_Topic_2.c_str(), TIME.c_str());
        Serial.println("Published Signal : " + TIME);
    }  
    else if(cmd.startsWith("PING_")){
        String val = cmd.substring(5);
        val.replace("S","");
        int sec = val.toInt();
        if(sec > 0){
            INT = sec * 1000UL;
            String result = "Time changed to " + String(sec) + " second";
            mqtt.publish(Pub_Topic_1.c_str(), result.c_str());
            Serial.println(result);     
        }
    }
    else if(cmd == "GET_CONFIG"){
      Serial.println("Command : GET_CONFIG");
        publishConfig(); 
       Serial.println("Command run successfully");             
    }
    else if(cmd=="ERASE_EEPROM"){
      Serial.println("Command : ERASE_EEPROM");
      eraseEEPROM();
      mqtt.publish(Pub_Topic_2.c_str(),"ERASED SUCCESFULLY...!");
    }
    else if(cmd=="RESTART_DEVICE"){
      Serial.println("Command : RESTART_DEVICE");
      ESP.restart();
      Serial.println("Restart Successfull");
      
    }    
    if(String(topic)=="M2627001_CONFIG"){ 
        mqtt.publish(Pub_Topic_2.c_str(),"value updating...");
      Serial.print("updating new config...");
      updateConfig(cmd);
    }
  }
