/*
 *  drv_dxl.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram, PBHP
 */

#include "drv_dxl.h"
#include "variant.h"





int drv_dxl_init()
{

  return 0;
}

void drv_dxl_begin(uint8_t channel)
{
  GPIO_InitTypeDef GPIO_InitStruct;


  switch(channel)
  {
    case 0:
      GPIO_InitStruct.Pin   = GPIO_PIN_5;
      GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull  = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
      break;

    case 1:
      break;

    case 2:
      GPIO_InitStruct.Pin   = GPIO_PIN_14;
      GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull  = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
      break;
  }
}

void drv_dxl_tx_enable(uint8_t channel, BOOL enable )
{
  switch(channel)
  {
    case 0:
      if( enable == TRUE )  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
      else                  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
      break;

    case 1:
      break;

    case 2:
      if( enable == TRUE )  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
      else                  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
      break;
  }
}
