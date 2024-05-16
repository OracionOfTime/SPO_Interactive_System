#include <ESP8266WiFi.h>
#include <espnow.h>

typedef struct struct_scale_check_result
{
    bool motor1;
    bool motor2;
    bool motor3;

}struct_scale_check_result;

struct_scale_check_result myData;


// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len);

void receiver_setup();

void receiver_loop();

