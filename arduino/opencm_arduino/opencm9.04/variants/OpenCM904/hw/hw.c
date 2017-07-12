/*
 *  hw.c
 *
 *  Created on: 2017. 7. 4.
 *      Author: Baram
 */

#include "hw.h"
#include "variant.h"



typedef struct
{
  uint8_t pin_config;
} hw_pin_config_t;

static hw_pin_config_t hw_pin_config[PINS_COUNT];




void hw_init(void)
{
  uint32_t i;


  swtimerInit();
  timerInit();

  drv_adc_init();
  drv_spi_init();
  drv_micros_init();
  drv_uart_init();
  drv_pwm_init();
  drv_timer_init();
  drv_i2c_init();
  drv_exti_init();
  drv_dxl_init();
  drv_eeprom_init();


  bsp_init_usb();

  for (i=0; i<5; i++)
  {
    bsp_led_on();
    delay(50);
    bsp_led_off();
    delay(50);
  }

  vcp_init();
  wdg_init();


  for (i=0; i<PINS_COUNT; i++)
  {
    hw_pin_config[i].pin_config = PIN_CONFIG_NONE;
  }
}

void hw_pin_release(uint32_t pin)
{
  if (pin >= PINS_COUNT)
  {
    return;
  }


  switch(hw_pin_config[pin].pin_config)
  {
    case PIN_CONFIG_PWM:
      drv_pwm_pin_release(pin);
      break;

    case PIN_CONFIG_ADC:
      drv_adc_pin_release(pin);
      break;
  }

  hw_pin_config[pin].pin_config = PIN_CONFIG_NONE;
}

void hw_pin_set_config(uint32_t pin, uint8_t pin_config)
{
  if (pin >= PINS_COUNT)
  {
    return;
  }


  hw_pin_config[pin].pin_config = pin_config;
}

uint8_t hw_pin_get_config(uint32_t pin)
{
  if (pin >= PINS_COUNT)
  {
    return PIN_CONFIG_NONE;
  }


  return hw_pin_config[pin].pin_config;
}

