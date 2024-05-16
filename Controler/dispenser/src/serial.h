#include "../../shared/structures.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <string>

class MySerial {
  public:
    MySerial(){};
    void init() { Serial.begin(115200); };

    void print_mac() {
        Serial.print("MAC address: ");
        Serial.println(WiFi.macAddress());
    }
    void print(std::vector<u8> data) {
        for (u8 ch : data) {
            Serial.write(ch);
        }
    }
    void print(std::string data) { Serial.write(data.c_str()); }
};