#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>
#include "dispenser_kits/motor.h"
#include "dispenser_kits/scale_HX711.h"
#include "HX711.h"
#include "wifi/mac_address.h"
#include "wifi/sender.h"
#include <ESP8266WiFi.h>
#include <espnow.h>

//#include "SoftwareSerial.h"
//#include <Adafruit_Fingerprint.h>
//#include "dispenser_kits/fingerprint.h"


// MAC address of the receiver board
uint8_t wristbandAddress[] = {0xE8, 0xDB, 0x84, 0x9B, 0xFC, 0xD7};

typedef struct struct_scale_check_result
{
    bool motor1;
    bool motor2;
    bool motor3;

}struct_scale_check_result;

struct_scale_check_result struct_scale_result;



Motor motor1(D5);
Motor motor2(D6);
Motor motor3(D7);

Scale scale(D4, D3);
unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send reading timer


long weight = 0;
long old_weight = 0;


bool dispense_and_check(Motor motor, int dosage, uint8_t LED)
{
  bool correct = false;

  if(dosage==0)
  {
    digitalWrite(LED, LOW);
  }
  else
  {
    digitalWrite(LED, HIGH);
  }

  motor.dispense_pill(dosage);
  old_weight = weight;
  weight = scale.get_weight();
  correct = scale.check_dosage(old_weight, dosage, weight);

  if(!correct)
  {
    Serial.println(" Not correct :(");
    Serial.print(correct);
  }
  else
  { 
    Serial.println(" Correct :)");
    Serial.print(correct);
  }

  return correct;
}



/*
SoftwareSerial mySerial(D2, D1);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;
*/

void setup() {
  // put your setup code here, to run once:

  // register receiver (wristband) ESP
  esp_now_add_peer(wristbandAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  
  getMAC_setup();
  pinMode(D0, OUTPUT); // for LED yellow
  digitalWrite(D0, LOW);
  pinMode(D1, OUTPUT); // for LED red
  digitalWrite(D1, LOW);
  pinMode(D2, OUTPUT); // for LED green
  digitalWrite(D2, LOW);

  // servo motors setup
  motor1.myServo.attach(D5, 500, 2400);
  motor1.myServo.write(0);
  motor2.myServo.attach(D6, 500, 2400);
  motor2.myServo.write(0);
  motor3.myServo.attach(D7, 500, 2400);
  motor3.myServo.write(0);
  
  pinMode(D8, INPUT); // for switch
  Serial.begin(115200);

  // scale setup
  
  scale.myScale.begin(D4, D3);
  scale.setup();

  // for sending scale-checker result
  sender_setup();
  esp_now_add_peer(wristbandAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  
  
  /*
  // fingerprint setup
   Serial.begin(115200);
   finger.begin(9600);
   finger_enroll_setup(id, finger);
   */
}

void loop() {
  // put your main code here, to run repeatedly:

  // for fingerprint
  // finger_enroll_loop(id, finger);

  // for scale calibration
  // scale.calibrate();

  //scale.print_weight();


  //delay(5000);

  
  // for motors to dispense pills
  if(digitalRead(D8) == HIGH)
  {  
    struct_scale_result.motor1 = dispense_and_check(motor1, 2, D0);
    struct_scale_result.motor2 = dispense_and_check(motor2, 1, D1);
    struct_scale_result.motor3 = dispense_and_check(motor3, 1, D2);
    Serial.println(struct_scale_result.motor1);
    Serial.println(struct_scale_result.motor2);
    Serial.println(struct_scale_result.motor3);
    weight = 0;
    old_weight = 0;
  }
  else
  {
    digitalWrite(D0, LOW); // for LED
    digitalWrite(D1, LOW); // for LED
    digitalWrite(D2, LOW); // for LED
  }
  
  
  // sending scale-checker result to wristband board
  if ((millis() - lastTime) > timerDelay) 
  {
    // Set values to send
    struct_scale_result.motor1 = random(1, 50);
    struct_scale_result.motor2 = random(1, 50);
    struct_scale_result.motor3 = random(1, 50);

    // Send message via ESP-NOW
    esp_now_send(wristbandAddress, (uint8_t *) &struct_scale_result, sizeof(struct_scale_result));
    lastTime = millis();
  }
}
