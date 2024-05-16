#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <Arduino.h>
#include <EEPROM.h>
#include <vector>

enum Unit { Pills = 0, MilliGrams, Grams };

enum Color { Green = 0, Red, Blue };

class Day {
  public:
    u8 val;
    Day() : val(0){};
    Day(bool b) : val(b ? 0xff : 0){};
    Day(bool morning, bool afternoon, bool evening)
        : val(morning * 0b001 + afternoon * 0b010 + evening * 0b0100){};
    Day(u8 val) : val(val) {}
    void set_morning() { val |= 0b001; };
    void set_afternoon() { val |= 0b010; };
    void set_evening() { val |= 0b100; };
    bool get_morning() { return val & 0b001; };
    bool get_afternoon() { return val & 0b010; };
    bool get_evening() { return val & 0b100; };
    bool operator[](u8 i) {
        switch (i % 3) {
        case 1:
            return this->get_morning();
        case 2:
            return this->get_afternoon();
        default:
            return this->get_evening();
        }
    }
};

class Entry {
  public:
    Color color;
    u8 dispenser_id;
    Unit unit;
    u8 unit_value;
    std::vector<Day> times;
    bool is_due(u8 weekday, u8 time) {
        Day day = this->times[weekday % times.size()];
        return day[time];
    }
    std::vector<u8> to_bytes() {
        u8 total = 4 + times.size();
        std::vector<u8> res = {};
        res.push_back(total);
        res.push_back(this->color);
        res.push_back(this->dispenser_id);
        res.push_back(this->unit);
        res.push_back(this->unit_value);
        for (auto time : times) {
            res.push_back(time.val);
        }
        return res;
    };
    template <typename T> Entry(T buffer, size_t *next) {
        size_t counter = *next;
        size_t total = buffer[counter++];
        this->color = (Color)buffer[counter++];
        this->dispenser_id = buffer[counter++];
        this->unit = (Unit)buffer[counter++];
        this->unit_value = buffer[counter++];
        this->times = {};
        for (size_t i = 0; i < total - 4; i++) {
            this->times.push_back(Day(buffer[counter++]));
        }
        *next = counter;
    }
};

struct DispenserMessage {
    u8 dispenser_id;
    u8 unit_value;
    Unit unit;
    u8 num_led;
};

typedef std::vector<Entry> Shedule;

u8 password[] = {0x42, 0x42, 0x56, 0x40};

#endif