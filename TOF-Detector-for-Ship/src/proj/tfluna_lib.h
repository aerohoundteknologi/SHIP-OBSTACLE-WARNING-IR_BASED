#ifndef TFLUNA_LIB_H
#define TFLUNA_LIB_H


#include <Arduino.h>
#include <ArduinoJson.h>
const int NUM_OF_DEG = 91;
const int NUM_OF_DATA = 3;
int16_t DATAS[NUM_OF_DEG][NUM_OF_DATA];

// prototyping
String printJsonProxData();
int16_t randomX();
int16_t randomY();


int16_t randomX() {
  return random(-200, 200);
}
int16_t randomY() {
  return random(50, 400);
}
int16_t randomDepth() {
  return random(0, 40);
}


String printJsonProxData() {
    const size_t bufferSize = JSON_ARRAY_SIZE(NUM_OF_DEG) + JSON_ARRAY_SIZE(NUM_OF_DATA) + NUM_OF_DEG * JSON_ARRAY_SIZE(NUM_OF_DATA);    
    DynamicJsonDocument doc(bufferSize);
    JsonObject root = doc.to<JsonObject>();
    root["event"] = "prox";

    JsonArray data = root.createNestedArray("result");
    for (int i = 0; i < NUM_OF_DEG; i++) {
    JsonArray subArray = data.createNestedArray();
    for (int j = 0; j < NUM_OF_DATA; j++) {
        subArray.add(DATAS[i][j]);
    }
    }

    // Serialize JSON to string
    String jsonString;
    serializeJson(doc, jsonString);

    // Display JSON in the Serial Monitor
    // Serial.println(jsonString);
    return jsonString;
}



#endif