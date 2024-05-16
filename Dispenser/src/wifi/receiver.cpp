#include "receiver.h"
#include <ESP8266WiFi.h>
#include <espnow.h>

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) 
{
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("result of the 1st slot: ");
  if(myData.motor1)
    Serial.println("Correct :)");
  else
    Serial.println("The output is not correct, please check the correct dosage!");
  Serial.print("result of the 2nd slot: ");
  if(myData.motor2)
    Serial.println("Correct :)");
  else
    Serial.println("The output is not correct, please check the correct dosage!");
  Serial.print("result of the 3rd slot: ");
  if(myData.motor3)
    Serial.println("Correct :)");
  else
    Serial.println("The output is not correct, please check the correct dosage!");
  Serial.println();
}


void receiver_setup()
{
    Serial.begin(115200);

    // set device as a wifi station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // initialize ESP-NOW
    if(esp_now_init() != 0)
    {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // register for receiver
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(OnDataRecv);

}


void receiver_loop() {
   // no need
}