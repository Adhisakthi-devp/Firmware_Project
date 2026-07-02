#pragma once
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ModbusMaster.h>
extern ModbusMaster node;
#include "config.h"
#include "config.h"
#include <RTClib.h>

extern RTC_DS3231 rtc;
#define EEPROM_SIZE 512

#define ADDR_DEVICE_ID       0
#define ADDR_SERVER_ADDR     50
#define ADDR_PORT            100
#define ADDR_APN             110
#define ADDR_PING_INTERVAL   160

#define ADDR_MB_SLAVE_ID     170
#define ADDR_MB_ADDRESS      180
#define ADDR_MB_VALUE        190
String Device_ID="DEV0001";
String Server_Address="iot.salieabs.in";
String APN ="jionet";



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
  uint8_t result;
  uint16_t buffer[1];
  result = node.readHoldingRegisters(0x0000, 1);
  if (result == node.ku8MBSuccess) {
    buffer[0] = node.getResponseBuffer(0);

    Serial.print("Register Value = ");
    Serial.print(buffer[0]);
    return buffer[0];
  } else {
    Serial.print("Modbus Error: 0x");
    Serial.println(result, HEX);
    return -1;
  }
}
void printConfig()
{
  
    Serial.println();
    Serial.println("========= CONFIGURATION =========");
    Serial.print("Device ID      : ");Serial.println(Device_ID);
    Serial.print("Server Address : ");Serial.println(Server_Address);
    Serial.print("APN            : ");Serial.println(APN);
    Serial.print("Port           : ");Serial.println(Port);


    Serial.print("Ping Interval  : ");Serial.println(Ping_Int);
    Serial.print("MB Slave ID    : ");Serial.println(MB_Slave_ID);
    Serial.print("MB Address     : ");Serial.println(MB_Address);

    Serial.println("==========================");  
}

void loadConfig()
{
    EEPROM.begin(EEPROM_SIZE);

    Device_ID      = EEPROM_ReadStr(ADDR_DEVICE_ID);
    Server_Address = EEPROM_ReadStr(ADDR_SERVER_ADDR);
    APN            = EEPROM_ReadStr(ADDR_APN);
    Port           = EEPROM_ReadInt(ADDR_PORT);
    
    MB_Slave_ID    = EEPROM_ReadInt(ADDR_MB_SLAVE_ID);
    MB_Address     = EEPROM_ReadInt(ADDR_MB_ADDRESS);
}
void eraseEEPROM(){
    for(int i=0;i<EEPROM_SIZE;i++){
        EEPROM.write(i,0);
    }
    EEPROM.commit();
}
String getDateTime() {
  DateTime now = rtc.now();
  char DT[25];
  sprintf(DT, "%02d-%02d-%04d %02d:%02d:%02d", now.day(),now.month(),now.year(), now.hour(), now.minute(), now.second());
  return String(DT);
}
void create_json() {
  Serial.println(" -Json Creation --");
  DynamicJsonDocument doc(200);
  doc["DeviceId "] = 1;
  doc["MacAddress "] = getMac();
  doc["DateTime "] = getDateTime();
  doc["RSSI "] = str;
  doc["MODBUS Value "] = getmodbusvalue();

  json = ""; 
  serializeJsonPretty(doc, json);
  Serial.println("Json Created: " + json);
}
