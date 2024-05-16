#include "../../shared/structures.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>

class MySerial {
  public:
    MySerial(){};
    void init() { Serial.begin(600000); };

    void print_mac() {
        Serial.print("MAC address: ");
        Serial.println(WiFi.macAddress());
    }
    void read(std::vector<u8> &buf) {
        while (Serial.available()) {
            u8 ch = Serial.read();
            buf.push_back(ch);
            digitalWrite(LED_BUILTIN, LOW);
        }
    }
    void write(std::vector<u8> buf) {
        for (u8 ch : buf) {
            Serial.write(ch);
        }
    }
};