#include "scale_HX711.h"
#include "HX711.h"
#include <EEPROM.h>

Scale::Scale(uint8_t DT_pin, uint8_t SCK_pin)
{
    DT = DT_pin;
    SCK = SCK_pin;
}

void Scale::calibrate()
{
    if(myScale.is_ready())
    {
        myScale.set_scale();
        Serial.println("Tare...remove any weights from the scale.");
        delay(5000);
        myScale.tare();
        Serial.println("Tare done...");
        Serial.print("Place a known weight on the scale...");
        delay(5000);
        long reading = myScale.get_units(10);
        Serial.print("Result: ");
        Serial.println(reading);
    }
    else
    {
        Serial.println("HX711 not found.");
    }
    delay(1000);
}

void Scale::setup()
{
    Serial.println("Initializing the scale HX711");       
    myScale.set_scale(-430.52667);
    myScale.tare();               // reset the scale to 0

    Serial.print("get units: \t\t");
    Serial.println(myScale.get_units(5), 1);        // print the average of 5 readings, divided by the SCALE parameter set with set_scale

    Serial.println("Readings:");
}

long Scale::get_weight()
{
    long weight;
    weight = myScale.get_units(5);
    Serial.print("Output weight:\t");
    Serial.print(weight);
    Serial.print("\t");
    return weight;
}

bool Scale::check_dosage(long old, int add_dosage, long output)
{
    long weight;
    weight = add_dosage * 1.1 + old;
    Serial.print("Old weight:\t");
    Serial.print(old);
    Serial.print("\t");
    Serial.print("Culculated weight:\t");
    Serial.print(weight);
    Serial.print("\t");

    if(weight - 0.6 < output && output < weight + 0.6)
       return true;
    else
       return false;
}
