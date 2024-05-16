#include <Arduino.h>
#include <Servo.h>

#ifndef MOTOR
#define MOTOR

class Motor  
{
  private:
    uint8_t id;
  
  public:
    Servo myServo;
    Motor(uint8_t id);

    uint8_t get_id() {return id;}

    void dispense_pill(int dosage);

    
};

#endif