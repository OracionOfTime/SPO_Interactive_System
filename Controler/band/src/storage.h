#include "../../shared/structures.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <vector>

class Storage {
  public:
    Storage() {}
    void init() { EEPROM.begin(128); }
    Storage(size_t size) { EEPROM.begin(size); }
    // not commiting
    void clear() {
        for (size_t i = 0; i < EEPROM.length(); i++) {
            EEPROM.write(i, 0);
        }
    }
    void write(Shedule shedule) {
        this->clear();
        std::vector<u8> buffer = {};
        for (auto entry : shedule) {
            std::vector<u8> data = entry.to_bytes();
            buffer.insert(buffer.end(), data.begin(), data.end());
        }
        int counter = 0;
        for (auto byte : buffer) {
            EEPROM.write(counter++, byte);
        }
        EEPROM.commit();
    }
    Shedule read() {
        size_t counter = 0;
        Shedule shedule = {};
        while (EEPROM.read(counter) != 0) {
            shedule.emplace_back(Entry(EEPROM, &counter));
        }
        return shedule;
    }
    void print() {
        for (size_t i = 0; i < EEPROM.length(); i++) {
            Serial.print(EEPROM.read(i));
            if (i % 20 == 0) {
                Serial.print("\n");
            }
        }
        Serial.print("\n");
    }
};