#pragma once
#include <PPPoS.h>
#include <ArduinoJson.h>
#include "config.h"
#include "EEPROM.h"

#include <RTClib.h>
extern RTC_DS3231 rtc;

#include <ModbusMaster.h>

void pppos_start();
void checking();

PPPoS ppp;
WiFiClient client;

#define txgsm 17
#define rxgsm 16 
#define engsm 32
#define pwrgsm 33

unsigned long INT = 5000;
void publishConfig();
void updateConfig(String json);


void checking(){
    if (!ppp.status())
    {
      Serial.println("PPP Lost");
      delay(2000);
      pppos_start();
    }
    
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



    String details;
    serializeJsonPretty(doc, details);
    Serial.println("Config Published:");
    Serial.println(details);
}

void message(char *topic, byte *payload, unsigned int length){
    String cmd = "";
    for (unsigned int i = 0; i < length; i++) { 
        cmd += (char)payload[i];
    }
    cmd.trim();
    Serial.println("Received Command : " + cmd);
    
    if (cmd == "GET_DATE"){
        String DATE = getD();
       
        Serial.println("Published Date : " + DATE);
    }
    else if (cmd == "GET_TIME"){
        String TIME = getT();
        
        Serial.println("Published Signal : " + TIME);
    }  
    else if(cmd.startsWith("PING_")){
        String val = cmd.substring(5);
        val.replace("S", "");
        int sec = val.toInt();
        if(sec > 0){
            INT = sec * 1000UL;
            String result = "Time changed to " + String(sec) + " second";
            Serial.println(result);     
        }
    }
    else if(cmd == "GET_CONFIG"){
        publishConfig();              
    }
    else if(cmd == "ERASE_EEPROM"){
      eraseEEPROM();
      
    }
    else if(cmd == "RESTART_DEVICE"){
      ESP.restart();
    }    
    if(String(topic) == "M2627001_CONFIG"){ 
      
      Serial.print("updating new config...");
      updateConfig(cmd);
    }
}

void pppos_start(){
    Serial.println("Starting PPPoS");
    Serial2.begin(115200, SERIAL_8N1, rxgsm, txgsm);
    ppp.begin(&Serial2);
    
    if (APN.length() == 0 || APN == "") {
        APN = "jionet"; 
        Serial.println("Notice: APN empty, forced fallback applied.");
    }
    
    ppp.connect(APN.c_str(), "", "");
    while (!ppp.status()){
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nPPP Connected");
    
    delay(2500); 
    
    Serial.print("Server : ");
    Serial.println(Server_Address);
  
    Serial.print("Port : ");
    Serial.println(Port);
      
    
}
