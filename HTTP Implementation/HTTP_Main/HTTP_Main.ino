/*
 
 NAME     : ADHI SAKTHI S
 DEPT.    : ELECTRONICS AND COMPUTER ENGINEERING 
 COLLEGE  : SONA COLLEGE OF TECHNOLOGY
 PROJECT TITLE  : IOT GATEWAY DEVELOPMENT
 
 */
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <RTClib.h>
#include <HTTPClient.h>

RTC_DS3231 rtc;
#include "config.h"
#include "EEPROM.h"
#include "MQTT.h"
#include "PPPoS_Init.h"

#define txModBus 15
#define rxModBus 4
#define enModBus 2 

ModbusMaster node;

void powering() {
  Serial.println("Powering GSM Module...");
  pinMode(engsm, OUTPUT);
  pinMode(pwrgsm, OUTPUT);
  
  digitalWrite(engsm, HIGH);
  delay(2000);
  digitalWrite(engsm, LOW);
  delay(2000);
  digitalWrite(pwrgsm, LOW);
  delay(2000);
  digitalWrite(pwrgsm, HIGH);
  delay(15000); // 15 seconds is usually enough for boot register
  Serial.println("Power ON complete");
  Serial2.begin(115200, SERIAL_8N1, rxgsm, txgsm);
}
void Modbus_Init(){
  Serial.println("Modbus Initialisation");
  Serial1.begin(9600, SERIAL_8N1, rxModBus, txModBus); 
  pinMode(enModBus, OUTPUT);
  digitalWrite(enModBus, LOW);
  
  node.begin(MB_Slave_ID, Serial1);
  node.preTransmission(modbus_pre);
  node.postTransmission(modbus_post);
  delay(2000);
  Serial.println("Modbus Initialisation completed..!");
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
void call(String cmd);
void at_commands() {
  Serial.println(" -- AT commands ---\n");
  call("AT");
  call("ATE0"); // Turn off echo to prevent loopback later
  call("AT+CPIN?");
  call("AT+COPS?");
  call("AT+CSQ");
  call("AT+CREG?");
  call("AT+CGDCONT=1,\"IP\",\"jionet\""); // Corrected Jio APN
  Serial.println("   -AT commands Passed Successfully   ---");
  call("ATD*99***1#"); 
  Serial2.end();
  delay(500);
}
void printing() {
  unsigned long start = millis();
  while (millis() - start < 2000) {
    while (Serial2.available()) {
      Serial.write(Serial2.read());
    }
  }
  Serial.println();
}
void call(String cmd) {
  Serial.print("Calling ");
  Serial.println(cmd);
  Serial2.println(cmd);
    delay(500);  
  printing()  ;
  if (cmd == "ATD*99***1#") {
    Serial.println(" -- Dialup  --");
    Serial2.println(cmd);
    unsigned long start = millis();
    while (millis() - start < 4000) {
      while (Serial2.available()) {
        String s = Serial2.readStringUntil('\n');
        s.trim();
        if (s.indexOf("CONNECT") >= 0) {
          Serial.println("    -Dialup Successful   ---");
          return;
        }
      }
    }
    Serial.println("    ---Dialup Failed    -");
    return;
  }  
  Serial2.println(cmd);
  printing();
}
void modbus_pre()  { digitalWrite(enModBus, HIGH); }
void modbus_post() { digitalWrite(enModBus, LOW); }
void testHTTP()
{
    HTTPClient http;

    http.begin("example.com");

    int httpCode = http.GET();

    Serial.print("HTTP Code: ");
    Serial.println(httpCode);

    if(httpCode > 0)
    {
        String response = http.getString();

        Serial.println("Response:");
        Serial.println(response);
    }
    else
    {
        Serial.print("Error: ");
        Serial.println(http.errorToString(httpCode));
    }

    http.end();
}
void setup() {
  Serial.begin(115200);
  delay(1000);
  loadConfig();
  printConfig();
  powering();
  RTC_setup();
  Modbus_Init();
  at_commands();
  create_json();
  pppos_start();

  if (ppp.status()){
    testHTTP();
  }
}
void loop() {
  checking(); 
  mqtt.loop();  
  static unsigned long last = 0;
  if (millis() - last > INT) {
    last = millis();    
    create_json();     
    mqtt.publish(Pub_Topic_1.c_str(), json.c_str());
  }
}
