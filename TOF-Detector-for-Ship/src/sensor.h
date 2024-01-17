#ifndef SENSOR_H
#define SENSOR_H
#include <Arduino.h>

#define LUNA_SERIAL Serial2

struct Sensor{
    float height;
    float draught;
    float alpha;
    float beta;
};

Sensor LUNA = {
    0.24,
    0.07,
    85.2,
    48.54,
};

// Prototyping
void lunaSetup();
float deg_to_rad(float deg);
void X(float& dist, float& depth, float C);
void tfluna_getdata(float& dist);


void lunaSetup(){
    LUNA_SERIAL.begin(115200); // Initializing serial port
}

uint16_t depth_status(float depth){
  if(depth < 0.14){
    return 2; // BAHAYA
  }else if(depth < 0.2){
    return 1; // AWAS
  }else{
    return 0; // AMAN
  }
}

float deg_to_rad(float deg){
    return deg * M_PI / 180.0;
}


void X(float& dist, float& depth, float C){
    float height_above_water = LUNA.height - LUNA.draught;
    float x1 = height_above_water * tan(deg_to_rad(LUNA.alpha));
    float c1 = sqrt(x1*x1 + height_above_water*height_above_water);

    if(C < c1){
        dist = sin(deg_to_rad(LUNA.alpha)) * C;
        depth = cos(deg_to_rad(LUNA.alpha))* C;

        return;
    }

    float c2 = C - c1;
    float x2 = sin(deg_to_rad(LUNA.beta)) * c2;
    depth = height_above_water+ cos(deg_to_rad(LUNA.beta))* c2;
    dist = x1 + x2;

}

int uart[9]; 
const int HEADER = 0x59;
int check;
float LUNA_DIST = 0;

float luna_update_data(){
  uint16_t dist;
  uint16_t strength;
  uint16_t temperature;
  int i = 0;
  if (LUNA_SERIAL.available())
  { // check if serial port has data input
    if (LUNA_SERIAL.read() == HEADER)
    { // assess data package frame header 0x59
      uart[0] = HEADER;
      if (LUNA_SERIAL.read() == HEADER)
      { // assess data package frame header 0x59
        uart[1] = HEADER;
        for (i = 2; i < 9; i++)
        { // save data in array
          uart[i] = LUNA_SERIAL.read();
        }
        check = uart[0] + uart[1] + uart[2] + uart[3] + uart[4] + uart[5] + uart[6] + uart[7];
        if (uart[8] == (check & 0xff))
        { 
          dist = uart[2] + uart[3] * 256;
          strength = uart[4] + uart[5] * 256;
          temperature = uart[6] + uart[7] * 256;
          temperature = temperature / 8 - 256;
        }
      }
    }
  }
  if(dist > 0){
    LUNA_DIST = dist;
    return dist;
  }else{
    return LUNA_DIST;
  }
}




#endif