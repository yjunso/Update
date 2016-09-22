//Alterations on VrTracker made by Sawyer Warden

#include <ESP8266httpUpdate.h> // For Server OTA
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <SPI.h> 
#include <Wire.h>
#include "PixySPI_SS.h"
#include <WebSocketsClient.h>
#include <ESP8266WiFiMulti.h>
//#include <Hash.h>

uint16_t blocks;
uint16_t old_blocks;

PixySPI_SS pixy(D8);

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

int counter=0;
char buf[10];
//unsigned long timedelay =0;

String macaddress = "";
IPAddress ip;
String GWIP = "192.168.0.25";
const char* mySSID = "KT_WLAN_2004";
const char* password = "vrgame123";
bool wifiConnectFailed = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n",  payload);
                webSocket.sendTXT("camera-" + macaddress);
            }
            break;
        case WStype_TEXT:
            Serial.printf("[WSc] get text: %s\n", payload);

      // send message to server
      // webSocket.sendTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary lenght: %u\n", lenght);
            hexdump(payload, lenght);

            // send data to server
            // webSocket.sendBIN(payload, lenght);
            break;
    }
}

void setup() {
    // USE_SERIAL.begin(921600);
    Serial.begin(115200);

    //Serial.setDebugOutput(true);
    Serial.setDebugOutput(true);
    Serial.println("VR Tracker by Jules Thuillier");
    Serial.println("Modified by Youngjun So");
    // Initialize camera
    pixy.init();
    
    byte mac[6];
    WiFi.macAddress(mac);
    char macadd[7];
    for(int i=0; i<6; i++){
        macadd[i] = (char)mac[i];
    }
    macaddress = String(macadd[0],HEX) + String(macadd[1],HEX) + String(macadd[2],HEX) + String(macadd[3],HEX) + String(macadd[4],HEX) + String(macadd[5],HEX);
    
    Serial.println();
    Serial.println();
    Serial.println();
    
    Serial.println("MA: " + macaddress);
    
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }
    
    //Add your wifi info in quotes, i.e. ("SSID", "password")
    WiFiMulti.addAP(mySSID, password);
    
    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    WSc_Begin();
  
}

void WSc_Begin(){
    //Add ip address of local server between quotes,
    // and change port if not using 8001
    webSocket.begin(GWIP, 8001);
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent);
}


void loop() {
  
  webSocket.loop();
  old_blocks = blocks;
  blocks = pixy.getBlocks();

  if (blocks && (old_blocks != blocks))
  { 
      for (int j=0; j<blocks; j++)
      {  
        if(pixy.blocks[j].width < 30 && pixy.blocks[j].height < 30){
          String data = "";
          char extract[2];
          sprintf(extract, "%d", pixy.blocks[j].signature);
          data += extract;
          data += 'x';
          sprintf(extract, "%d", pixy.blocks[j].x);
          data += extract;
          data += 'y';
          sprintf(extract, "%d", pixy.blocks[j].y);
          data += extract;
          data += 'h';
          sprintf(extract, "%d", pixy.blocks[j].width);
          data += extract;
          data += 'w';
          sprintf(extract, "%d", pixy.blocks[j].height);
          data += extract;
          data += 'a';
          sprintf(extract, "%d", pixy.blocks[j].angle);
          data += extract;
        //  Serial.println(data);
          webSocket.sendTXT(data);
        }
      }   
  }  
  
  else {
      delayMicroseconds(20);
  } 
  
}
