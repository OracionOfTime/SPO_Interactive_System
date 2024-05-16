
#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <Arduino.h>
#include <functional>

uint8_t getFingerprintID();

void setup_fingerprint();

bool fingerprint_loop(std::function<void()> before,
                      std::function<void()> after);

uint8_t getFingerprintEnroll(
    std::function<void()> before = []() {},
    std::function<void()> after = []() {});

int getFingerprintIDez();

void setup_enroll();

void enroll_loop(uint16_t _id, std::function<void()> before,
                 std::function<void()> after);

uint16_t get_num_prints();
#endif
