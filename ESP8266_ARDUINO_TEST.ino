// Arduino 1.0.6

// #define _SS_MAX_RX_BUFF 64 // RX buffer size => 256
// SoftwareSerial.h Изменить размер RX буфера.

// ESP 8266 Version

// AT version:0.22.0.0(Mar 20 2015 10:04:26)
// SDK version:1.0.0
// compile time:Mar 20 2015 11:00:32

//  AT+CWSAP_DEF="GPS","",6,0
//  AT+CWSAP_CUR="GPS","",6,0

//  AT+CWMODE=3  Soft_AP + Client
// AT+CIPMUX=1 Много соединений
// AT+CIPSERVER=1,80  Сервер на 80 порту

// https://github.com/itead/ITEADLIB_Arduino_WeeESP8266
// 
// #define USER_SEL_VERSION	VERSION_22
// #define ESP8266_USE_SOFTWARE_SERIAL

// http://www.atmel.com/webdoc/AVRLibcReferenceManual/index.html
// Много хороших функций

#include "ESP8266.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2,3); // RX, TX

#define SSID                "OS"
#define PASSWORD    "4957392716"

ESP8266 wifi(mySerial);

uint8_t buffer[128] = {
  0};
uint8_t mux_id;
uint32_t len = 0;

char pip[16],aip[16];
char s[128];
const char delimiters[] = "\":";

void setup(void)
{

  pinMode(A0,OUTPUT);
  digitalWrite(A0,LOW);

  Serial.begin(57600);
  mySerial.begin(57600);

  wifi.setOprToStationSoftAP();

  if (wifi.joinAP(SSID, PASSWORD)) {
    strcpy(s,wifi.getLocalIP().c_str());
    char *token;

    token = strtok (s, delimiters);     

    while(token != NULL) {
      if  (strstr(token,"PIP")) strcpy(pip,strtok (NULL, delimiters)); 
      if  (strstr(token,"AIP")) strcpy(aip,strtok (NULL, delimiters)); 
      token = strtok (NULL, delimiters);   
    }
  }

  wifi.enableMUX();
  wifi.startTCPServer(80);
  wifi.setTCPServerTimeout(10);

}


void loop(void)
{

  digitalWrite(A0,HIGH);

  if (mySerial.available()) {
    len = wifi.recv(&mux_id, buffer, sizeof(buffer), 100);
  }

  digitalWrite(A0,LOW);

  if (len > 0) {
    strcpy(s,"<HTML><HEAD>");
    wifi.send(mux_id, (const uint8_t*)s, strlen(s) );
    strcpy(s,"</HEAD><BODY>");
    wifi.send(mux_id, (const uint8_t*)pip, strlen(pip) );
    strcpy(s,"<P>");
    wifi.send(mux_id, (const uint8_t*)s, strlen(s) );
    wifi.send(mux_id, (const uint8_t*)aip, strlen(aip) );
    strcpy(s,"</BODY></HTML>");
    wifi.send(mux_id, (const uint8_t*)s, strlen(s) );
    wifi.releaseTCP(mux_id);
    len = 0;
  }

}


