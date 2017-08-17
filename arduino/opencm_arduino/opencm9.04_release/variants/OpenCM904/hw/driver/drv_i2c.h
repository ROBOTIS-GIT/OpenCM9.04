/*
 *  drv_i2c.h
 *
 *  Created on: 2016. 7.13.
 *      Author: Baram, PBHP
 */

#ifndef DRV_I2C_H
#define DRV_I2C_H


#ifdef __cplusplus
 extern "C" {
#endif


#include "def.h"
#include "bsp.h"








int drv_i2c_init();

void drv_i2c_begin(uint8_t ch, uint32_t freq_khz);
void drv_i2c_reset(uint8_t ch);

int16_t drv_i2c_read(uint8_t ch, uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length,  uint16_t timeout);
int16_t drv_i2c_read16(uint8_t ch, uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint8_t length, uint16_t timeout);
int16_t drv_i2c_write(uint8_t ch, uint8_t devAddr, uint8_t regAddr, uint8_t* pData, uint8_t length);
int16_t drv_i2c_write16(uint8_t ch, uint8_t devAddr, uint8_t regAddr, uint16_t* pData, uint8_t length);


#ifdef __cplusplus
}
#endif


#endif
