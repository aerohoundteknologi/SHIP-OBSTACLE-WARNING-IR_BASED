#ifndef WEBSOCKSERV_H
#define WEBSOCKSERV_H
#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>

#define USE_SERIAL Serial

const uint16_t WS_serverPort = 2345;
WebSocketsServer webSocket = WebSocketsServer(WS_serverPort);


// Prototyping
void websocketSetup();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);


void websocketSetup() {
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        webSocket.sendTXT(num, "{\"event\":\"connected\"}");
      }
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
      // You can add custom handling for received text from clients here
      break;
    case WStype_BIN:
      USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
      // You can add custom handling for received binary data from clients here
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

#endif