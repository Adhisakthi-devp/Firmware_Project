#include <PubSubClient.h>
#include <RTClib.h>

#include <ModbusMaster.h>
extern PubSubClient mqtt;

extern unsigned long INT;
extern RTC_DS3231 rtc;

extern ModbusMaster node;

extern String Device_ID;
extern String Server_Address;
extern String APN;

extern String MQTT_User;
extern String MQTT_Password;
extern String MQTT_ClientId;

extern String Pub_Topic_1;
extern String Pub_Topic_2;
extern String Sub_Topic;

extern uint16_t Port;
extern uint16_t Ping_Int;
extern uint16_t MB_Slave_ID;
extern uint16_t MB_Address;

extern String WIFI_SSID;
extern String WIFI_PASSWORD;


String getD();
String getT();
                                                          
#define txModBus  15
#define rxModBus  4
#define enModBus  2
