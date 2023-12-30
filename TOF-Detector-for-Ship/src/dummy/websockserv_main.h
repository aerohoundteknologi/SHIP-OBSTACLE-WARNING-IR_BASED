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

#define SEND_DELAY 1000

// prototypes function
void sendDataToClients(String data);

// ssid and password for connection
const char *w_ssid = "Informatics ITS 20";
const char *w_password = "informatics";



WiFiMulti wifiConn;





void setup() {
  USE_SERIAL.begin(115200);
  
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
  USE_SERIAL.println(ip);

  websocketSetup();
}

void loop() {
  webSocket.loop();


  sendDataToClients("Hello from ESP32");
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
