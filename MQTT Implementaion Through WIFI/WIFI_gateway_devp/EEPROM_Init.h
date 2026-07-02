#pragma once
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ModbusMaster.h>
extern ModbusMaster node;
#include "config.h"
#include <RTClib.h>

extern RTC_DS3231 rtc;

#define EEPROM_SIZE 1080

#define ADDR_DEVICE_ID       0
#define ADDR_SERVER_ADDR     50
#define ADDR_PORT            100
#define ADDR_APN             110
#define ADDR_PING_INTERVAL   160

#define ADDR_MB_SLAVE_ID     170
#define ADDR_MB_ADDRESS      180
#define ADDR_MB_VALUE        190

#define ADDR_Pub_TOPIC_1  200
#define ADDR_Pub_TOPIC_2  260
#define ADDR_Sub_TOPIC  320
#define ADDR_MQTT_User 380
#define ADDR_MQTT_Password 430
#define ADDR_MQTT_ClientId 480
#define ADDR_WIFI_SSID 550
#define ADDR_WIFI_PASSWORD 650


String Device_ID="M2627001";
String Server_Address="your-Server-addres";
String APN ="jionet";
String MQTT_User="YYY";
String MQTT_Password="XXX";
String MQTT_ClientId = "Device-M2627001";
String Pub_Topic_1="out1";
String Pub_Topic_2="out2";
String Sub_Topic="in";
String WIFI_SSID = "your ssid";
String WIFI_PASSWORD = "your-password";
uint16_t Ping_Int=5;
uint16_t MB_Slave_ID=1  ;
uint16_t MB_Address =0;
uint16_t Port =1883;
String json = "";
int str = 0;




String getMac() {
  return WiFi.macAddress();
}
bool EEPROM_WriteInt(int addr,uint16_t val){
   EEPROM.put(addr, val);
   return EEPROM.commit();
  
}
uint16_t EEPROM_ReadInt(int addr)
{
    uint16_t val;
    EEPROM.get(addr, val);
    return val;
}

bool EEPROM_WriteStr(int addr,String ele){
  int len = ele.length();
  for(int i=0 ;i<len;i++){
    EEPROM.write(addr+i ,ele[i]);
  }
  EEPROM.write(addr + len, '\0'); 

  return EEPROM.commit();
}

String EEPROM_ReadStr(int addr){
  String res="";
  while(addr < EEPROM_SIZE){
    char ch= EEPROM.read(addr);
    if(ch=='\0'){
      break;
    }
    res+=ch;addr++;;
  }
  return res; 
}
void setServerAdd(String NewServer,uint16_t newPort){
  Serial.print("Server updating\n");
  EEPROM_WriteStr(ADDR_SERVER_ADDR,NewServer);
  EEPROM_WriteInt(ADDR_PORT,newPort);
  Serial.print("Server  Successfully setted\n");
  Server_Address=NewServer;Port=newPort;  
}

void setAPN(String newAPN){
  Serial.print("Setting APN..\n");
  EEPROM_WriteStr(ADDR_APN,newAPN);
  Serial.print("APN Successfully setted\n");
  APN=newAPN;
}

void setDeviceID(String newID){
  Serial.print("Device ID updating\n");
  EEPROM_WriteStr(ADDR_DEVICE_ID,newID);
  Device_ID = newID;
  Serial.print(newID);
  Serial.print('\n');
  
}

int getmodbusvalue() {
  uint16_t val = 0;
  // DYNAMIC FIX: Polling register mapped dynamically from variable instead of 0x0000
  uint8_t res = node.readHoldingRegisters(MB_Address, 1);
  if (res == node.ku8MBSuccess) {
    val = node.getResponseBuffer(0);
    return val;
  }
  else return -1;
}
String getDateTime() {
  DateTime now = rtc.now();
  char DT[25];
  sprintf(DT, "%02d-%02d-%04d %02d:%02d:%02d", now.day(),now.month(),now.year(), now.hour(), now.minute(), now.second());
  return String(DT);
}

void printConfig()
{
    Serial.println();
    Serial.println("========= CONFIGURATION =========");
    Serial.print("Device ID      : ");Serial.println(Device_ID);
    Serial.print("Server Address : ");Serial.println(Server_Address);
    Serial.print("APN            : ");Serial.println(APN);
    Serial.print("Port           : ");Serial.println(Port);
    Serial.print("MQTT UserNAME  : ");Serial.println(MQTT_User);
    Serial.print("MQTT Password : ");Serial.println(MQTT_Password);
    Serial.print("MQTT ClientId : ");Serial.println(MQTT_ClientId);

    Serial.print("Ping Interval  : ");Serial.println(Ping_Int);
    Serial.print("MB Slave ID    : ");Serial.println(MB_Slave_ID);
    Serial.print("MB Address     : ");Serial.println(MB_Address);
    Serial.print("Pub Topic      : ");Serial.println(Pub_Topic_1);
    Serial.print("Pub Topic      : ");Serial.println(Pub_Topic_2);
    Serial.print("Sub Topic      : ");Serial.println(Sub_Topic);
    Serial.println("==========================");  
}

void loadConfig()
{
    EEPROM.begin(EEPROM_SIZE);

    Device_ID      = EEPROM_ReadStr(ADDR_DEVICE_ID);
    Server_Address = EEPROM_ReadStr(ADDR_SERVER_ADDR);
    APN            = EEPROM_ReadStr(ADDR_APN);
    Port           = EEPROM_ReadInt(ADDR_PORT);
    MQTT_User="YYY";
    MQTT_Password="XXX";
    MQTT_ClientId = "Device-M2627001";
    
    Ping_Int       = EEPROM_ReadInt(ADDR_PING_INTERVAL);
    MB_Slave_ID    = EEPROM_ReadInt(ADDR_MB_SLAVE_ID);
    MB_Address     = EEPROM_ReadInt(ADDR_MB_ADDRESS);
    Pub_Topic_1    = "out1";
    Pub_Topic_2    = "out2";
    Sub_Topic      = "in";
}
void eraseEEPROM(){
    for(int i=0;i<EEPROM_SIZE;i++){
        EEPROM.write(i,0);
    }
    EEPROM.commit();
}
void create_json() {  
  StaticJsonDocument<200> doc;
  doc["DeviceId "] = 1;
  doc["MacAddress "] = getMac();
  doc["DateTime "] = getDateTime();
  doc["RSSI "] = WiFi.RSSI();
  doc["MODBUS Value "] = getmodbusvalue();
  json = ""; 
  serializeJsonPretty(doc, json);
}
