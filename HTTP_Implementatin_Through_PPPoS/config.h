#pragma once
#include <ModbusMaster.h>

extern unsigned long INT;
extern RTC_DS3231 rtc;

extern ModbusMaster node;

extern String Device_ID;
extern String Server_Address;
extern String APN;

extern uint16_t Port;
extern uint16_t Ping_Int;
extern uint16_t MB_Slave_ID;
extern uint16_t MB_Address;

String getD();
String getT();

#define txgsm 17
#define rxgsm 16
#define engsm 32
#define pwrgsm 33
                                                          
#define txModBus 15
#define rxModBus 4
#define enModBus 2
