/*
 *  drv_dxl.h
 *
 *  Created on: 2016. 7.23.
 *      Author: Baram, PBHP
 */

#ifndef DRV_DXL_H
#define DRV_DXL_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "def.h"
#include "bsp.h"







int drv_dxl_init();
void drv_dxl_begin(uint8_t channel);
void drv_dxl_tx_enable(uint8_t channel, BOOL enable );



#ifdef __cplusplus
}
#endif


#endif
