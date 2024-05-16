#include "sender.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    char macStr[18];
    Serial.print("Packet to:");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
             mac_addr[5]);
    Serial.print(macStr);
    Serial.print(" send status: ");
    if (sendStatus == 0) {
        Serial.println("Delivery success");
    } else {
        Serial.println("Delivery fail");
    }
}

void sender_setup() {
    Serial.begin(115200);

    // set device as a WIFI Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // initialize ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // set sender board role
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

    esp_now_register_send_cb(OnDataSent);
}
