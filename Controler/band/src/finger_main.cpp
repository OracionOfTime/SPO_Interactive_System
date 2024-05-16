#include <Arduino.h>
#include <fingerprint.h>


void main_setup() {
  setup_fingerprint();
}

void main_loop() {
  fingerprint_loop();
}
