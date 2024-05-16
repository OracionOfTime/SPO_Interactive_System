#include "display.h"
#include "fingerprint.h"
#include "serial.h"
#include "storage.h"
#include "wireless.h"
#include <Arduino.h>
#include <string>

MySerial serial;
u8 TimeCounter;
u8 EntryCounter;
uint8_t ReceiverAddress[] = {0xE8, 0xDB, 0x84, 0x9B, 0xC6, 0xAB};
uint8_t led_pins[3] = {D5, D6, D7};
uint8_t button_pin = D8;
uint8_t buzzer_pin = D0;
Shedule shedule = {};
Storage storage;

void message_sent(u8 *mac, u8 status);
void message_recived(uint8_t *incommingMacAddress, uint8_t *incomingData,
                     uint8_t len);
Display display;

Wireless wireless(ReceiverAddress);

void receive_shedule();

/***
 * Pin Layout
 * D0 -> Buzzer
 * D1, D2 -> I2C (Monitor)
 * D5, D6, D7 -> Color LEDs
 * D8 -> Next Button
 */

void setup() {
    // Section LEDS: 0: GREEN, 1: RED, 2: BLUE
    for (int i = 0; i <= 2; i++) {
        pinMode(led_pins[i], OUTPUT);
        digitalWrite(led_pins[i], LOW);
    }

    // Section next button
    pinMode(button_pin, INPUT);

    // Section Serial
    serial.init();

    // Section Wireless
    wireless.init(message_recived, message_sent);

    // Section Storage
    storage.init();

    // Section buzzer
    pinMode(buzzer_pin, OUTPUT);
    digitalWrite(buzzer_pin, LOW);

    // Section Counter
    TimeCounter = 0;
    EntryCounter = 0;

    // Section Display
    display.init();
}

void message_sent(u8 *mac, u8 status) { wireless.message_sent(mac, status); }
void message_recived(uint8_t *incommingMacAddress, uint8_t *incomingData,
                     uint8_t len) {
    wireless.message_recived(incommingMacAddress, incomingData, len);
}

void receive_shedule() {}

bool success_dispense = true;

void send_next_dosage() {
    digitalWrite(led_pins[0], LOW);
    digitalWrite(led_pins[1], LOW);
    digitalWrite(led_pins[2], LOW);
    display.clear();
    u8 day = TimeCounter / 3;
    u8 time = TimeCounter % 3;
    EntryCounter = EntryCounter >= shedule.size() ? 0 : EntryCounter;
    Entry entry = shedule[EntryCounter++];
    if (entry.is_due(day, time)) {
        digitalWrite(buzzer_pin, HIGH);

        switch (entry.color) {
        case Green:
            digitalWrite(led_pins[0], HIGH);
            break;
        case Red:
            digitalWrite(led_pins[1], HIGH);
            break;
        case Blue:
            digitalWrite(led_pins[2], HIGH);
            break;
        }
        display.display(entry);
        delay(1000);
        DispenserMessage msg;
        msg.dispenser_id = entry.dispenser_id;
        msg.unit = entry.unit;
        msg.unit_value = entry.unit_value;
        msg.num_led = entry.color;
        if (wireless.send_till_success(msg)) {
            Serial.println("sucessfully dispensed!");
            digitalWrite(D0, LOW);
            switch (entry.color) {
            case Green:
                digitalWrite(led_pins[0], LOW);
                break;
            case Red:
                digitalWrite(led_pins[1], LOW);
                break;
            case Blue:
                digitalWrite(led_pins[2], LOW);
                break;
            }
            success_dispense = true;
        } else {
            Serial.println("Could not dispense properly");
            success_dispense = false;
        }

    } else {
        Serial.println("dosage not due");
    }
};

void repeat_dosage() {
    EntryCounter--;
    send_next_dosage();
}

void start_buzz() {}

void stop_buzz() {}

void show_dosage() {}

void alert_time() {}

std::string shedule_to_string(Shedule shedule) {
    std::string res = "";
    for (auto entry : shedule) {
        res += ", Color ";
        res += entry.color + '0';
        res += ", dispenser id ";
        res += entry.dispenser_id + '0';
        res += ", unit ";
        res += entry.unit + '0';
        res += ", unit value ";
        res += entry.unit_value + '0';
        int counter = 1;
        for (auto day : entry.times) {
            res += ", day ";
            res += counter++ + '0';
            res += ", Morning ";
            res += day.get_morning() + '0';
            res += ", afternoon ";
            res += day.get_evening() + '0';
            res += ", evening ";
            res += day.get_afternoon() + '0';
        }
        res += "\n";
    }
    return res;
}

std::vector<u8> data = {};

void loop() {
    // put your main code here, to run repeatedly:
    if (data.empty()) {
        serial.read(data);
    }
    if (!data.empty()) {
        size_t index = 0;
        Shedule _shedule = {};
        while (index < data.size() && data[index] != 0) {
            _shedule.push_back(Entry(data, &index));
        }
        storage.clear();
        // storage.print();
        storage.write(_shedule);
        // storage.print();
        Serial.println("saved");
        shedule = _shedule;
        Serial.println(shedule_to_string(shedule).c_str());
        data.clear();
        delay(1000);
    } else if (shedule.empty()) {
        // storage.print();
        shedule = storage.read();
        Serial.println("Loaded");
        Serial.println(shedule_to_string(shedule).c_str());
        delay(1000);
    }
    if (digitalRead(button_pin) == HIGH && success_dispense) {
        Serial.println("sending next dosage");
        send_next_dosage();
    } else if (digitalRead(button_pin) == HIGH && !success_dispense) {
        digitalWrite(buzzer_pin, LOW);
        digitalWrite(led_pins[0], LOW);
        digitalWrite(led_pins[1], LOW);
        digitalWrite(led_pins[2], LOW);
        display.clear();
        success_dispense = true;
        Serial.println("Manual dosage taken");
        delay(1000);
    }
}