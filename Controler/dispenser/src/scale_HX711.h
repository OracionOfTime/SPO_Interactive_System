#include <Arduino.h>
#include <EEPROM.h>
#include "HX711.h"


#ifndef SCALE
#define SCALE

class Scale  
{   private:
    uint8_t DT;
    uint8_t SCK;
    //-108627.8/250.8 = -432.7761
    //-32282/75
    long calibration_factor = -430.42667;

    public:
    HX711 myScale;
    Scale(uint8_t DT_pin, uint8_t SCK_pin);

    uint8_t get_DT() {return DT;}
    uint8_t get_SCK() {return SCK;}

    void calibrate();
    void setup();

    long get_weight();

    bool check_dosage(long old, int add_dosage, long output);
};


#endif
