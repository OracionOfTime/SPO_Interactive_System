#include <ESP8266WiFi.h>

// Dispenser ESP Board MAC Address:  E8:DB:84:9B:C6:AB

void getMAC_setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}