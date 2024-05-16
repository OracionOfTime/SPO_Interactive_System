#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>


// callback function that will be executed when a message is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus);

void sender_setup();