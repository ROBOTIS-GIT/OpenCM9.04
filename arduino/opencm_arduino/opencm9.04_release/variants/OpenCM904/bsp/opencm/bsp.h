/*
 *  bsp.h
 *
 *  boart support package
 *
 *  Created on: 2017. 3. 16.
 *      Author: Baram
 */

#ifndef BSP_H
#define BSP_H


#ifdef __cplusplus
 extern "C" {
#endif


#include <stdint.h>

#include "def.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"


#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"




void bsp_led_on(void);
void bsp_led_off(void);



 void bsp_init();
 void bsp_init(void);
 void bsp_init_usb(void);



#ifdef __cplusplus
}
#endif
#endif
