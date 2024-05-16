#include "../../shared/structures.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

class Wireless {
  private:
    u8 *mac;
    DispenserMessage messageIn;
    u8 messageOut;
    void (*callback)(DispenserMessage);
    int status = 0;

  public:
    void message_sent(u8 *mac, u8 stat) {
        if (stat == 0) {
            status = 0;
            Serial.println("Erfolgreich gesendet");
        } else {
            status = -1;
            Serial.println("Fehler beim senden");
        }
    }
    void message_received(uint8_t *incommingMacAddress, uint8_t *incomingData,
                          uint8_t len) {
        Serial.println("message received");
        memcpy(&this->messageIn, incomingData, sizeof(this->messageIn));
        this->callback(this->messageIn);
    }
    Wireless(u8 *receiver_mac, void (*callback)(DispenserMessage))
        : mac(receiver_mac), callback(callback) {}
    void init(void (*recv_handler)(uint8_t *, uint8_t *, uint8_t),
              void (*send_handler)(u8 *mac, u8 status)) {
        WiFi.mode(WIFI_STA);
        if (esp_now_init() != 0) {
            Serial.println("Error initializing ESP-NOW");
        }
        esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
        esp_now_add_peer(this->mac, ESP_NOW_ROLE_COMBO, 1, password, 4);
        esp_now_register_recv_cb(recv_handler);
        esp_now_register_send_cb(send_handler);
    }
    void print_own_mac() { Serial.println(WiFi.macAddress()); }
    void send(u8 msg) {
        memcpy(&this->messageOut, (u8 *)&msg, sizeof(this->messageOut));
        esp_now_send(this->mac, (u8 *)&this->messageOut,
                     sizeof(this->messageOut));
    }
    bool pending() { return status == 1; }
    bool failed() { return status == -1; }
    bool ready() { return status == 0; }
    void send_till_success(u8 msg) {
        send(msg);

        while (!ready()) {
            if (!pending()) {
                send(msg);
                delay(500);
            }
            delay(10);
        }

        status = 0;
    }
};