#ifndef WEBSOCKSERV_MAIN_H
#define WEBSOCKSERV_MAIN_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsServer.h>
#include "../websockserv.h"
#include "tfluna_lib.h"
#include "../sensor.h"
#include <ESP32Servo.h>


#ifndef USE_SERIAL
#define USE_SERIAL Serial
#endif

#define SEND_DELAY 15

// SERVO
const int pin_servo = 13;
Servo servo1;

void SERVO_SETUP(){
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo1.setPeriodHertz(50);    // standard 50 hz servo
  servo1.attach(pin_servo, 500, 2400); // attaches the servo on pin 13 to the servo object
}


const int fov = 90;
int angle_open = 0;
// prototypes function
void sendDataToClients(String data);
void sendDataToClients_CSV(int angle);
void sendDataToClients_CSV_wdlay(String data);

// ssid and password for connection
const char *w_ssid = "Comrade";
const char *w_password = "putin001";



WiFiMulti wifiConn;


void setup() {
  // SERVO
  SERVO_SETUP();
  angle_open = 90+(fov/2);
  servo1.write(angle_open);
  
  USE_SERIAL.begin(115200);
  lunaSetup();
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

int FLOAT_PRECISION = 5;

void loop() {
  webSocket.loop();


  // Yaopo yo carae ngekeki delay? ohh reti
  // forward 
  for( int i = 0; i < 91; i++){
    webSocket.loop();
    float C = luna_update_data();
    C/=100.0;
    float dist;
    float depth;
    X(dist, depth, C);

    String Data = String(i)+","+ \
    String(dist, FLOAT_PRECISION )+","+ \
    String(depth, FLOAT_PRECISION)+","+ \
    String(C, FLOAT_PRECISION);


    sendDataToClients_CSV_wdlay(Data);
    servo1.write(angle_open-i);
    // sendDataToClients_CSV(i);
  }
  // backward
  for( int i = 90; i >= 0; i--){
    webSocket.loop();
    float C = luna_update_data();
    C/=100.0;
    float dist;
    float depth;
    X(dist, depth, C);

    String Data = String(i)+","+ \
    String(dist, FLOAT_PRECISION )+","+ \
    String(depth, FLOAT_PRECISION)+","+ \
    String(C, FLOAT_PRECISION);

    sendDataToClients_CSV_wdlay(Data);
    servo1.write(angle_open-i);
    // sendDataToClients_CSV(i);
  }
}

void sendDataToClients_CSV_wdlay( String data) {
  // kita akan berikan layer while untuk delay pada baris ke 2
  static uint32_t lastMillis = millis();
  while(1){
    webSocket.loop();
    uint32_t currentMillis = millis();
    if (currentMillis - lastMillis >= SEND_DELAY) {
      bool sending = webSocket.broadcastTXT(data);
      lastMillis = currentMillis;
      return;
    }
  }
}

void sendDataToClients_CSV(int angle){
  int16_t dist = random(0, 600);
  int16_t depth = random(0, 40);
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
