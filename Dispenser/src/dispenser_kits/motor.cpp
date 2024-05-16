#include "motor.h"
#include <Servo.h>

// pin number as servo ID
Motor::Motor(uint8_t servo_id)
{
    id = servo_id;
}

void Motor::dispense_pill(int dosage)
{
  //please do not change these values, 
  //otherwise our dispenser will be brocken !!!
  const int START_POSITION = 0;
  const int END_POSITION = 84;
  int pos = START_POSITION;
  int speed = 4; // {1, 2, 4, 6}

  for(int t = 0; t < dosage; t++)
  {
    for(pos = 0; pos <= END_POSITION; pos += speed)
    {
      myServo.write(pos);
    }
  
    for(pos = 84; pos >= START_POSITION; pos -= speed)
    {
      myServo.write(pos);
    }
    
    pos = START_POSITION;  
  }
}