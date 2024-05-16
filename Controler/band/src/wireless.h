#include "../../shared/structures.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <vector>

class Wireless {
  private:
    u8 *mac;
    u8 messageIn;
    DispenserMessage messageOut;
    int status = 0;
    int rec = 0;

  public:
    void message_sent(u8 *mac, u8 stat) {
        if (stat == 0) {
            Serial.println("Erfolgreich gesendet");
            status = 0;
        } else {
            Serial.println("Fehler beim senden");
            status = -1;
        }
    }
    void message_recived(uint8_t *incommingMacAddress, uint8_t *incomingData,
                         uint8_t len) {
        memcpy(&this->messageIn, incomingData, sizeof(this->messageIn));
        if (this->messageIn != 0) {
            Serial.println("Erfolgreich ausgeführt");
            rec = 1;
        } else {
            Serial.println("Fehler beim ausführen");
            rec = -1;
        }
    }

    bool pending() { return status == 1; }
    bool failed() { return status == -1; }
    bool ready() { return status == 0; }

    Wireless(u8 *receiver_mac) : mac(receiver_mac) {}
    void init(void (*recv_handler)(uint8_t *, uint8_t *, uint8_t),
              void (*sent_handler)(u8 *, u8)) {
        WiFi.mode(WIFI_STA);
        if (esp_now_init() != 0) {
            Serial.println("Error initializing ESP-NOW");
        }
        esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
        esp_now_add_peer(this->mac, ESP_NOW_ROLE_COMBO, 1, password, 4);
        esp_now_register_recv_cb(recv_handler);
        esp_now_register_send_cb(sent_handler);
    }
    void print_own_mac() { Serial.println(WiFi.macAddress()); }
    void send(DispenserMessage msg) {
        memcpy(&this->messageOut, (u8 *)&msg, sizeof(this->messageOut));
        status = 1;
        esp_now_send(this->mac, (u8 *)&this->messageOut,
                     sizeof(this->messageOut));
    }
    bool send_till_success(DispenserMessage msg) {
        send(msg);
        return true;
    }
};