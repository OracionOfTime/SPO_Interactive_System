#include "HX711.h"
#include "Servo.h"
#include "SoftwareSerial.h"
#include "fingerprint.h"
#include "motor.h"
#include "scale_HX711.h"
#include "serial.h"
#include "wireless.h"
#include <Adafruit_Fingerprint.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <espnow.h>

#define NUM_SERVOS 3

// #define SCALE_AVAILABLE // uncomment if scale should be used

bool button_pressed;
MySerial serial;
bool to_dispense = false;
int a, b;

unsigned long lastTime = 0;
unsigned long timerDelay = 2000; // send reading timer

long weight = 0;
long old_weight = 0;

void dispense_dosage(DispenserMessage msg);

void dispense_pill();

u8 mac_address[] = {0xE8, 0xDB, 0x84, 0x9B, 0xFC, 0xD7};

void message_received(uint8_t *incommingMacAddress, uint8_t *incomingData,
                      uint8_t len);

void message_sent(u8 *mac, u8 status);

Motor motor1(D5);
Motor motor2(D6);
Motor motor3(D7);

u8 reserve[] = {10, 10, 10};
#ifdef SCALE_AVAILABLE
const u8 scalePorts[] = {TX, RX};
Scale scale(scalePorts[0], scalePorts[1]);
#endif
const u8 num_motors = 3;
const u8 LEDPorts[] = {D0, D1, D2};

bool dispensed = true;
u8 current_motor = 0;
int current_dosage = 0;
u8 current_led = 0;

void dispense_handler(DispenserMessage msg) {
    if (!dispensed) {
        return;
    }
    dispensed = false;

    current_motor = msg.dispenser_id - 1;
    Serial.print("Motor: ");
    Serial.println(current_motor);
    current_dosage = msg.unit_value;
    current_led = msg.num_led;
}

bool dispense_and_check(Motor motor, int dosage, uint8_t LED) {
    bool correct = true;

    if (dosage == 0) {
        digitalWrite(LED, LOW);
    } else {
        digitalWrite(LED, HIGH);
    }

    motor.dispense_pill(dosage);
    old_weight = weight;
#ifdef SCALE_AVAILABLE
    weight = scale.get_weight();
    correct = scale.check_dosage(old_weight, dosage, weight);
#endif
    if (!correct) {
        Serial.println(" Not correct :(");
        Serial.print(correct);
    } else {
        Serial.println(" Correct :)");
        Serial.print(correct);
    }

    return correct;
}

Wireless wireless(mac_address, dispense_handler);

void message_received(uint8_t *incommingMacAddress, uint8_t *incomingData,
                      uint8_t len) {
    wireless.message_received(incommingMacAddress, incomingData, len);
}

void message_sent(u8 *mac, u8 status) { wireless.message_sent(mac, status); }

void dispense_dosage() {}

void weight_dosage() {}

void show_success() {}

void show_error() {}

unsigned long blink_time = 0;

const std::function<void()> red_on = []() { digitalWrite(D1, HIGH); };
const std::function<void()> red_off = []() { digitalWrite(D1, LOW); };
const std::function<void()> green_on = []() { digitalWrite(D0, HIGH); };
const std::function<void()> green_off = []() { digitalWrite(D0, LOW); };

void setup() {
    serial.init();
    pinMode(LEDPorts[0], OUTPUT); // for LED yellow
    digitalWrite(LEDPorts[0], LOW);
    pinMode(LEDPorts[1], OUTPUT); // for LED red
    digitalWrite(LEDPorts[1], LOW);
    pinMode(LEDPorts[2], OUTPUT); // for LED green
    digitalWrite(LEDPorts[2], LOW);

    uint8_t v1 = motor1.myServo.attach(D5, 500, 2400);
    motor1.myServo.write(0);
    uint8_t v2 = motor2.myServo.attach(D6, 500, 2400);
    motor2.myServo.write(0);
    uint8_t v3 = motor3.myServo.attach(D7, 500, 2400);
    motor3.myServo.write(0);

    if (v1 == 0 || v2 == 0 || v3 == 0) {
        Serial.println("Attaching of Servo failed!");
        exit(0);
    }

    pinMode(D8, INPUT);
    button_pressed = false;

#ifdef SCALE_AVAILABLE
    scale.myScale.begin(scalePorts[0], scalePorts[1]);
    scale.setup();
#endif
    wireless.init(message_received, message_sent);

    setup_fingerprint();
    blink_time = millis();
}

bool blink_on = false;

void loop() {
    // put your main code here, to run repeatedly:

    /*if (digitalRead(D8) == LOW) {
        button_pressed = false;
    }

    if (digitalRead(D8) == HIGH && button_pressed == false) {
        dispense_pill();
        button_pressed = true;
    }

    while (a > 0) {
        dispensed = true;
        dispense_pill();
        Serial.print(a);
        a--;
        delay(1000);
    }
    if (dispensed) {
        dispensed = false;
        wireless.send_till_success(1);
    }*/

    if (!dispensed) {

        fingerprint_loop(red_on, red_off);
        bool res = true;
        if (reserve[current_motor] < current_dosage) {
            res = false;
        }
        if (res) {
            digitalWrite(LEDPorts[current_led % num_motors], HIGH);
            delay(1000);
            digitalWrite(LEDPorts[current_led % num_motors], LOW);
            dispensed = true;
            if (current_motor == 0) {
                res = dispense_and_check(motor1, current_dosage, current_led);
                reserve[0] -= current_dosage;
            } else if (current_motor == 1) {
                res = dispense_and_check(motor2, current_dosage, current_led);
                reserve[1] -= current_dosage;
            } else {
                res = dispense_and_check(motor3, current_dosage, current_led);
                reserve[2] -= current_dosage;
            }
        }

        dispensed = true;
        wireless.send_till_success(res ? 0 : 1);
    }
    auto t = millis();

    if (digitalRead(D8)) {
        if (t < 1000 * 30) { // enroll new fingerprint
            Serial.println("enrolling fingerprint");
            enroll_loop(get_num_prints(), green_on, green_off);
        } else {
            bool needs_refill = reserve[0] <= 3 || reserve[1] <= 3 || reserve[2] <= 3;
            while (digitalRead(D8) == HIGH) { // wait for button release
                delay(10);
            }
            if (needs_refill) {
                fingerprint_loop(red_on, red_off); // wait for valid finger to authorise refill

                reserve[0] = 10;
                reserve[1] = 10;
                reserve[2] = 10;

                blink_on = true;
                while (digitalRead(D8)) {
                    blink_on = !blink_on;
                    auto mode = blink_on ? LOW : HIGH;
                    digitalWrite(LEDPorts[0], mode);
                    digitalWrite(LEDPorts[1], mode);
                    digitalWrite(LEDPorts[2], mode);
                    delay(100);
                }
                if (digitalRead(D8) == HIGH) {
                    Serial.println("Chambers have been refilled");
                    digitalWrite(LEDPorts[0], LOW);
                    digitalWrite(LEDPorts[1], LOW);
                    digitalWrite(LEDPorts[2], LOW);
                }
            }
        }
    }

    if ((t - blink_time) > 200) {
        Serial.println("Switching blinking");
        blink_on = !blink_on;
        blink_time = t;
    }
    auto mode = blink_on ? LOW : HIGH;
    if (reserve[0] <= 3) {
        digitalWrite(LEDPorts[0], mode);
    }
    if (reserve[1] <= 3) {
        digitalWrite(LEDPorts[1], mode);
    }
    if (reserve[2] <= 3) {
        digitalWrite(LEDPorts[2], mode);
    }

    /*if (digitalRead(D8) == HIGH) {
        bool res1 = dispense_and_check(motor1, 2, LEDPorts[0]);
        bool res2 = dispense_and_check(motor2, 1, LEDPorts[1]);
        bool res3 = dispense_and_check(motor3, 1, LEDPorts[2]);
        Serial.println(res1);
        Serial.println(res2);
        Serial.println(res3);
        weight = 0;
        old_weight = 0;

    } else {
        digitalWrite(LEDPorts[0], LOW); // for LED
        digitalWrite(LEDPorts[1], LOW); // for LED
        digitalWrite(LEDPorts[2], LOW); // for LED
    }*/

    // // sending scale-checker result to wristband board
    // if ((millis() - lastTime) > timerDelay) {
    //     // Set values to send
    //     struct_scale_result.motor1 = random(1, 50);
    //     struct_scale_result.motor2 = random(1, 50);
    //     struct_scale_result.motor3 = random(1, 50);

    //     // Send message via ESP-NOW
    //     esp_now_send(wristbandAddress, (uint8_t *)&struct_scale_result,
    //                  sizeof(struct_scale_result));
    //     lastTime = millis();
    // }
}
