/*
 *  hw.h
 *
 *  Created on: 2017. 7. 4.
 *      Author: Baram
 */

#ifndef HW_H
#define HW_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "def.h"
#include "bsp.h"


#include "drv_adc.h"
#include "drv_pwm.h"
#include "drv_spi.h"
#include "drv_uart.h"
#include "drv_i2c.h"
#include "drv_exti.h"
#include "drv_dxl.h"
#include "drv_timer.h"
#include "drv_eeprom.h"
#include "drv_micros.h"

#include "timer.h"
#include "swtimer.h"
#include "delay.h"
#include "vcp.h"
#include "wdg.h"



void hw_init(void);
void hw_pin_release(uint32_t pin);

void    hw_pin_set_config(uint32_t pin, uint8_t pin_config);
uint8_t hw_pin_get_config(uint32_t pin);

#ifdef __cplusplus
}
#endif


#endif
