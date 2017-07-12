/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/



#include <Arduino.h>
#include <Servo.h>



#define SERVO_MIN() (MIN_PULSE_WIDTH - this->min)  // minimum value in uS for this servo
#define SERVO_MAX() (MAX_PULSE_WIDTH - this->max)  // maximum value in uS for this servo








Servo::Servo()
{
  servoIndex = 0;
  pulse = 0;
}

uint8_t Servo::attach(int pin)
{
  return this->attach(pin, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
}

uint8_t Servo::attach(int pin, int min, int max)
{
  this->servoIndex = pin;
  this->min  = (MIN_PULSE_WIDTH - min); //resolution of min/max is 4 uS
  this->max  = (MAX_PULSE_WIDTH - max);


  drv_pwm_setup(pin, 1000000, REFRESH_INTERVAL);

  return this->servoIndex;
}

void Servo::detach()
{
  drv_pwm_pin_release(this->servoIndex);
}

void Servo::write(int value)
{
  // treat values less than 544 as angles in degrees (valid values in microseconds are handled as microseconds)
  if (value < MIN_PULSE_WIDTH)
  {
    if (value < 0)
      value = 0;
    else if (value > 180)
      value = 180;

    value = map(value, 0, 180, SERVO_MIN(), SERVO_MAX());
  }
  writeMicroseconds(value);
}

void Servo::writeMicroseconds(int value)
{
  // calculate and store the values for the given channel
  byte channel = this->servoIndex;


  if (value < SERVO_MIN())          // ensure pulse width is valid
    value = SERVO_MIN();
  else if (value > SERVO_MAX())
    value = SERVO_MAX();

  pulse = value;
  drv_pwm_set_data(channel, value);
}

int Servo::read() // return the value as degrees
{
  return map(readMicroseconds()+1, SERVO_MIN(), SERVO_MAX(), 0, 180);
}

int Servo::readMicroseconds()
{
  return pulse;
}

bool Servo::attached()
{
  return (bool)drv_pwm_get_init(this->servoIndex);
}



