#ifndef WEBSOCKSERV_MAIN_H
#define WEBSOCKSERV_MAIN_H

// built-in library
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP32Servo.h>
// custom library
#include "../websockserv.h"
#include "../sensor.h"



#ifndef USE_SERIAL
#define USE_SERIAL Serial
#endif

#define SEND_DELAY 20

// SERVO
const int pin_servo = 13;
const int fov = 46;
int angle_open = 0;
Servo servo1;

void SERVO_SETUP(){
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo1.setPeriodHertz(50);    // standard 50 hz servo
  servo1.attach(pin_servo, 500, 2400); // attaches the servo on pin 13 to the servo object
}




// prototypes function
void sendDataToClients_CSV_wdlay(String data);

// WIFI SETUP
// ssid and password for connection
const char *w_ssid = "ABCDEFG";
const char *w_password = "123456789";

WiFiMulti wifiConn;


void setup() {
  // SERVO
  SERVO_SETUP();
  angle_open = 90+(fov/2);
  servo1.write(angle_open);
  
  USE_SERIAL.begin(115200);
  lunaSetup();
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
  USE_SERIAL.print("Address\t: ws://");USE_SERIAL.print(ip);
  USE_SERIAL.print(":");
  USE_SERIAL.println(WS_serverPort);
  websocketSetup();
}


int FLOAT_PRECISION = 5;
void loop() {
  webSocket.loop();
  // forward 
  for( int i = 0; i < (fov+1); i++){
    webSocket.loop();
    servo1.write(angle_open-i);
    float C = luna_update_data();
    C/=100.0;
    float dist;
    float depth;
    X(dist, depth, C);
    // depth status
    uint16_t d_status = depth_status(depth);
    String Data = String(i)+","+ \
    String(dist, FLOAT_PRECISION )+","+ \
    String(d_status);
    sendDataToClients_CSV_wdlay(Data);
  }
  // backward
  for( int i = fov; i >= 0; i--){
    servo1.write(angle_open-i);
    webSocket.loop();
    float C = luna_update_data();
    C/=100.0;
    float dist;
    float depth;
    X(dist, depth, C);
    // depth status
    uint16_t d_status = depth_status(depth);
    String Data = String(i)+","+ \
    String(dist, FLOAT_PRECISION )+","+ \
    String(d_status);
    sendDataToClients_CSV_wdlay(Data);
  }
}

// send data with delay
void sendDataToClients_CSV_wdlay( String data) {
  // kita akan berikan layer while untuk delay
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

#endif
