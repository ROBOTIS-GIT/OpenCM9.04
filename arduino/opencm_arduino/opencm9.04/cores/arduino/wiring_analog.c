/*
   Copyright (c) 2011 Arduino.  All right reserved.
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU Lesser General Public License for more details.
   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "Arduino.h"
#include "variant.h"

uint8_t analog_reference = DEFAULT;


static int _readResolution = 10;
static int _writeResolution = 8;
int readResolBackup = -1;
int writeResolBackup = -1;



void analogReadResolution(int res) {
	_readResolution = res;
}


void analogWriteResolution(int res) {
	_writeResolution = res;
}

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to) {
	if (from == to)
		return value;
	if (from > to)
		return value >> (from-to);
	else
		return value << (to-from);
}

void analogReference(uint8_t mode){
	analog_reference = mode;
}

uint32_t analogRead( uint32_t ulPin ){
  int32_t adc_value = 0;


  adc_value = drv_adc_read(ulPin);
  if (adc_value < 0)
  {
    return 0;
  }
  else
  {
    adc_value = mapResolution(adc_value, 12, _readResolution);
  }

  return adc_value;
}

void analogWrite( uint32_t ulPin, uint32_t ulValue ){

  if( drv_pwm_get_init(ulPin) == false )
  {
    uint32_t res_data;

    res_data = (1 << _writeResolution) - 1;

    drv_pwm_setup(ulPin, 10000*res_data, res_data); // 10Khz, 0~_writeResolution
  }

  drv_pwm_set_duty(ulPin, _writeResolution, ulValue);
}

void analogWriteFreq( uint32_t ulPin, uint32_t pwmFreq){
  drv_pwm_setup_freq(ulPin, pwmFreq);
}
