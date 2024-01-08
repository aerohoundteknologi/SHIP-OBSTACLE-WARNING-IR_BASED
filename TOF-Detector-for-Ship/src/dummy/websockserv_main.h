#ifndef WEBSOCKSERV_MAIN_H
#define WEBSOCKSERV_MAIN_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsServer.h>
#include "../websockserv.h"
#include "tfluna_dummy.h"


#ifndef USE_SERIAL
#define USE_SERIAL Serial
#endif

#define SEND_DELAY 200

// prototypes function
void sendDataToClients(String data);
void sendDataToClients_CSV(int angle);
void sendDataToClients_CSV_wdlay(String data);

// ssid and password for connection
const char *w_ssid = "Informatics ITS 20";
const char *w_password = "informatics";



WiFiMulti wifiConn;
const int fov = 46;
int angle_open = 0;

void setup() {
  USE_SERIAL.begin(115200);
  angle_open = 90+(fov/2);
  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(100);
  }

  wifiConn.addAP(w_ssid, w_password);
  USE_SERIAL.println("Connecting to WiFi");
  while (wifiConn.run() != WL_CONNECTED) {
    delay(100);
  }
  USE_SERIAL.println("Connected to WiFi");
  // Print ESP32 IP address
  IPAddress ip = WiFi.localIP();
  USE_SERIAL.print("IP Address: ");
  USE_SERIAL.print(ip);
  USE_SERIAL.print(":");
  USE_SERIAL.println(WS_serverPort);

  websocketSetup();
}

void loop() {
  webSocket.loop();


  // Yaopo yo carae ngekeki delay? ohh reti
  // forward 
  for( int i = 0; i < (fov+1); i++){
    sendDataToClients_CSV(i);
  }
  // backward
  for( int i = fov; i >= 0; i--){
    sendDataToClients_CSV(i);
  }
}

void sendDataToClients_CSV_wdlay( String data) {
  // kita akan berikan layer while untuk delay pada baris ke 2
  static uint32_t lastMillis = millis();
  while(1){
    webSocket.loop();
    uint32_t currentMillis = millis();
    // Serial.print(currentMillis);
    // Serial.print(" - ");
    // Serial.print(lastMillis);
    // Serial.print(" = ");
    // Serial.println(currentMillis - lastMillis);
    if (currentMillis - lastMillis >= SEND_DELAY) {
      bool sending = webSocket.broadcastTXT(data);
      // Serial.println("Update Last Millis");
      lastMillis = currentMillis;
      return;
    }
  }
}

void sendDataToClients_CSV(int angle){
  int16_t dist = random(0, 600);
  int16_t depth = random(0, 2);
  String data = String(angle)+","+String(dist)+","+String(depth);
  sendDataToClients_CSV_wdlay(data);
}

void sendDataToClients( String data) {
  static uint32_t lastMillis = 0;
  uint32_t currentMillis = millis();

  if (currentMillis - lastMillis >= SEND_DELAY) {
    String data = printJsonProxData();
    // USE_SERIAL.println(data);
    bool sending = webSocket.broadcastTXT(data);
    lastMillis = currentMillis;
    if (sending == true){

      // ngerandom data :D
      uint16_t rand = random(0,90);
      DATAS[rand][0] = randomX();
      DATAS[rand][1] = randomY();
      DATAS[rand][2] = randomDepth();
    }
  }
  
}

#endif
