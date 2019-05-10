/*
 *  drv_i2c.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram, PBHP
 */

#include "drv_i2c.h"
#include "variant.h"


#define I2C_MAX_CH    2


static I2C_HandleTypeDef I2cHandle[I2C_MAX_CH];

static uint32_t i2c_freq[I2C_MAX_CH];



void drv_i2c_setup_gpio(uint8_t ch);



int drv_i2c_init()
{
  return 0;
}

void drv_i2c_begin(uint8_t ch, uint32_t freq_khz)
{
  I2C_HandleTypeDef *p_handle;


  i2c_freq[ch] = freq_khz;

  if (ch == 0)
  {
    p_handle = &I2cHandle[0];
    p_handle->Instance = I2C1;
  }
  else
  {
    p_handle = &I2cHandle[1];
    p_handle->Instance = I2C2;
  }

  p_handle->Init.ClockSpeed      = freq_khz * 1000;
  p_handle->Init.DutyCycle       = I2C_DUTYCYCLE_2;
  p_handle->Init.OwnAddress1     = 0x00;
  p_handle->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  p_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  p_handle->Init.OwnAddress2     = 0xFF;
  p_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  p_handle->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

  HAL_I2C_DeInit(p_handle);
  if(HAL_I2C_Init(p_handle) != HAL_OK)
  {
  }
}

void drv_i2c_recovery(uint8_t ch)
{
  //SET_BIT(I2cHandle[ch].Instance->CR1,I2C_CR1_STOP);
  SET_BIT(I2cHandle[ch].Instance->CR1,I2C_CR1_SWRST);
  drv_i2c_begin(ch, i2c_freq[ch]);
}

int16_t drv_i2c_read(uint8_t ch, uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint8_t length,  uint16_t timeout)
{
#if 1
  HAL_I2C_Master_Transmit(&I2cHandle[ch], devAddr << 1, &regAddr, 1, timeout);

  if (HAL_I2C_Master_Receive(&I2cHandle[ch], devAddr << 1, data, length, timeout) == HAL_OK)
  {
    return length;
  }
  else
  {
    //vcp_printf("err\n");
    //vcp_printf("recovery...\n");
    drv_i2c_reset(ch);
    drv_i2c_recovery(ch);
    return -1;
  }
#else
  uint32_t pre_time;

  pre_time = millis();


  while(HAL_I2C_Master_Transmit_IT(&I2cHandle[ch], devAddr << 1, (uint8_t*)&regAddr, 1)!= HAL_OK)
  {
    if (HAL_I2C_GetError(&I2cHandle[1]) != HAL_I2C_ERROR_AF)
    {
      return -1;
    }
    if (millis()-pre_time > timeout)
    {
      return -1;
    }
  }

  while (HAL_I2C_GetState(&I2cHandle[ch]) != HAL_I2C_STATE_READY)
  {
    if (millis()-pre_time > timeout)
    {
      return -1;
    }
  }

  while(HAL_I2C_Master_Receive_IT(&I2cHandle[1], devAddr << 1, (uint8_t *)data, length) != HAL_OK)
  {
    if (HAL_I2C_GetError(&I2cHandle[1]) != HAL_I2C_ERROR_AF)
    {
      return -1;
    }
    if (millis()-pre_time > timeout)
    {
      return -1;
    }
  }

  while (HAL_I2C_GetState(&I2cHandle[ch]) != HAL_I2C_STATE_READY)
  {
    if (millis()-pre_time > timeout)
    {
      return -1;
    }
  }
  return length;
#endif
}

int16_t drv_i2c_read16(uint8_t ch, uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint8_t length, uint16_t timeout)
{
#if 1
  HAL_I2C_Master_Transmit(&I2cHandle[ch], devAddr << 1, &regAddr, 1, timeout);
  if (HAL_I2C_Master_Receive(&I2cHandle[ch], devAddr << 1, (uint8_t *)data, length*2, timeout) == HAL_OK)
  {
      return length;
  }
  else
  {
    return -1;
  }
#else
  uint32_t pre_time;

  pre_time = millis();

  while(HAL_I2C_Master_Transmit_IT(&I2cHandle[ch], devAddr << 1, (uint8_t*)&regAddr, 1)!= HAL_OK)
  {
    if (HAL_I2C_GetError(&I2cHandle[1]) != HAL_I2C_ERROR_AF)
    {
      return -1;
    }
    if (millis()-pre_time > timeout)
    {
      return -1;
    }
  }

  while (HAL_I2C_GetState(&I2cHandle[ch]) != HAL_I2C_STATE_READY)
  {
    if (millis()-pre_time > timeout)
    {
      return -1;
    }
  }

  while(HAL_I2C_Master_Receive_IT(&I2cHandle[1], devAddr << 1, (uint8_t *)data, length*2) != HAL_OK)
  {
    if (HAL_I2C_GetError(&I2cHandle[1]) != HAL_I2C_ERROR_AF)
    {
      return -1;
    }
    if (millis()-pre_time > timeout)
    {
      return -1;
    }
  }

  while (HAL_I2C_GetState(&I2cHandle[ch]) != HAL_I2C_STATE_READY)
  {
    if (millis()-pre_time > timeout)
    {
      return -1;
    }
  }

  return length;
#endif
}

int16_t drv_i2c_write(uint8_t ch, uint8_t devAddr, uint8_t regAddr, uint8_t* pData, uint8_t length)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&I2cHandle[ch], devAddr << 1, regAddr, I2C_MEMADD_SIZE_8BIT, pData, length, 100);

    return status == HAL_OK;
}

int16_t drv_i2c_write16(uint8_t ch, uint8_t devAddr, uint8_t regAddr, uint16_t* pData, uint8_t length)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Mem_Write(&I2cHandle[ch], devAddr << 1, regAddr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)pData, sizeof(uint16_t) * length, 1000);

    return status == HAL_OK;
}

void drv_i2c_reset(uint8_t ch)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  if (ch == 0)
  {
    __HAL_AFIO_REMAP_I2C1_DISABLE();

    GPIO_InitStruct.Pin       = GPIO_PIN_8;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = GPIO_PIN_9;
    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, HIGH);
    for (uint8_t i=0; i<9; i++)
    {
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, HIGH);
  }

  if (ch == 1)
  {
    GPIO_InitStruct.Pin       = GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin       = GPIO_PIN_11;
    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, HIGH);
    for (uint8_t i=0; i<9; i++)
    {
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
    }
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, HIGH);
  }
}

void drv_i2c_setup_gpio(uint8_t ch)
{
  GPIO_InitTypeDef  GPIO_InitStruct;


  if (ch == 0)
  {
    __HAL_AFIO_REMAP_I2C1_ENABLE();


    GPIO_InitStruct.Pin       = GPIO_PIN_8;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C SDA GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }

  if (ch == 1)
  {
    GPIO_InitStruct.Pin       = GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C SDA GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}


//-- I2C1
//
void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&I2cHandle[0]);
}

void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&I2cHandle[0]);
}


//-- I2C2
//
void I2C2_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&I2cHandle[1]);
}

void I2C2_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&I2cHandle[1]);
}



void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *I2cHandle)
{
  //vcp_printf("err callback : %d\n", I2cHandle->ErrorCode);
}


void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  GPIO_InitStruct;


  if( hi2c->Instance == I2C1 )
  {
    drv_i2c_reset(0);

    /* Enable I2Cx clock */
    __HAL_RCC_I2C1_CLK_ENABLE();


    /*##-3- Configure peripheral GPIO ##########################################*/
    /* I2C SCL GPIO pin configuration  */
    drv_i2c_setup_gpio(0);

    /*##-3- Configure the NVIC for I2C ########################################*/
    /* NVIC for I2Cx */
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 1);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 2);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
  }
  if( hi2c->Instance == I2C2 )
  {
    drv_i2c_reset(1);

    /* Enable I2Cx clock */
    __HAL_RCC_I2C2_CLK_ENABLE();


    /*##-3- Configure peripheral GPIO ##########################################*/
    /* I2C SCL GPIO pin configuration  */
    drv_i2c_setup_gpio(1);


    /*##-3- Configure the NVIC for I2C ########################################*/
    /* NVIC for I2Cx */
    HAL_NVIC_SetPriority(I2C2_ER_IRQn, 5, 1);
    HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
    HAL_NVIC_SetPriority(I2C2_EV_IRQn, 5, 2);
    HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
  }
}

/**
  * @brief I2C MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hi2c: I2C handle pointer
  * @retval None
  */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
  if( hi2c->Instance == I2C1 )
  {
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_I2C1_FORCE_RESET();
    __HAL_RCC_I2C1_RELEASE_RESET();
    __HAL_RCC_I2C1_CLK_DISABLE();

    __HAL_AFIO_REMAP_I2C1_DISABLE();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure I2C SCL as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);
    /* Configure I2C SDA as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);

    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
  }
  if( hi2c->Instance == I2C2 )
  {
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_I2C2_FORCE_RESET();
    __HAL_RCC_I2C2_RELEASE_RESET();
    __HAL_RCC_I2C2_CLK_DISABLE();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure I2C SCL as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
    /* Configure I2C SDA as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);

    HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
    HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
  }
}
