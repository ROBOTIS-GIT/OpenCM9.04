/*
 *  drv_adc.c
 *
 *  Created on: 2016. 5. 14.
 *      Author: Baram, PBHP
 */

#include "drv_adc.h"
#include "variant.h"






ADC_HandleTypeDef hADC1;



int drv_adc_init()
{
  hADC1.Instance                   = ADC1;
  hADC1.Init.ScanConvMode          = DISABLE;
  hADC1.Init.ContinuousConvMode    = DISABLE;
  hADC1.Init.DiscontinuousConvMode = DISABLE;
  hADC1.Init.NbrOfDiscConversion   = 0;
  hADC1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hADC1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hADC1.Init.NbrOfConversion       = 1;

  if (HAL_ADC_Init(&hADC1) != HAL_OK)
  {
    return -1;
  }

  HAL_ADCEx_Calibration_Start(&hADC1);

  return 0;
}


void drv_adc_pin_init( uint32_t ulPin )
{
  GPIO_InitTypeDef GPIO_InitStruct;


  if( g_Pin2PortMapArray[ulPin].GPIOx_Port == NULL ) return;


  HAL_GPIO_DeInit(g_Pin2PortMapArray[ulPin].GPIOx_Port, g_Pin2PortMapArray[ulPin].Pin_abstraction);

  GPIO_InitStruct.Pin = g_Pin2PortMapArray[ulPin].Pin_abstraction;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(g_Pin2PortMapArray[ulPin].GPIOx_Port, &GPIO_InitStruct);
}

void drv_adc_pin_release(uint32_t ulPin)
{
  if (ulPin < PINS_COUNT)
  {

  }
}

int32_t drv_adc_read(uint32_t ulPin)
{
  ADC_ChannelConfTypeDef sConfig;
  ADC_HandleTypeDef      *hADCx;
  uint32_t ulValue = 0;
  uint32_t ulChannel;
  uint32_t adc_pin;


  adc_pin = analogPinToChannel(ulPin);

  ulChannel = g_Pin2PortMapArray[adc_pin].adc_channel;

  if(ulChannel == NO_ADC)
      return -1;

  hADCx = g_Pin2PortMapArray[adc_pin].ADCx;

  sConfig.Channel      = ulChannel;
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;

  HAL_ADC_ConfigChannel(hADCx, &sConfig);

  HAL_ADC_Start(hADCx);
  HAL_ADC_PollForConversion(hADCx, 10);
  ulValue = HAL_ADC_GetValue(hADCx);

  return (int32_t)ulValue;
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  uint8_t i;
  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInit;


  if( hadc->Instance == ADC1 )
  {
    __HAL_RCC_ADC1_CLK_ENABLE();
  }

  /* Configure ADCx clock prescaler */
  /* Caution: On STM32F1, ADC clock frequency max is 14MHz (refer to device   */
  /*          datasheet).                                                     */
  /*          Therefore, ADC clock prescaler must be configured in function   */
  /*          of ADC clock source frequency to remain below this maximum      */
  /*          frequency.                                                      */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);


  i = 0;
  while(1)
  {
    if( g_Pin2PortMapArray[i].GPIOx_Port == NULL ) break;

    HAL_GPIO_DeInit(g_Pin2PortMapArray[i].GPIOx_Port, g_Pin2PortMapArray[i].Pin_abstraction);

    GPIO_InitStruct.Pin = g_Pin2PortMapArray[i].Pin_abstraction;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(g_Pin2PortMapArray[i].GPIOx_Port, &GPIO_InitStruct);

    i++;
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  uint8_t i;


  if( hadc->Instance == ADC1 )
  {
    __HAL_RCC_ADC1_FORCE_RESET();
    __HAL_RCC_ADC1_RELEASE_RESET();
    __HAL_RCC_ADC1_CLK_DISABLE();
  }


  i = 0;
  while(1)
  {
    if( g_Pin2PortMapArray[i].GPIOx_Port == NULL ) break;

    HAL_GPIO_DeInit(g_Pin2PortMapArray[i].GPIOx_Port, g_Pin2PortMapArray[i].Pin_abstraction);

    i++;
  }
}
