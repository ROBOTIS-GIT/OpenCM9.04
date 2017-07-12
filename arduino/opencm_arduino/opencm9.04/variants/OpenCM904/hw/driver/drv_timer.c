/*
 *  drv_timer.c
 *
 *  Created on: 2016. 9. 23.
 *      Author: Baram, PBHP
 */
#include "drv_timer.h"
#include "variant.h"


/*
 TIMER_CH1  TIM1
 TIMER_CH2  TIM2
 TIMER_CH3  TIM3
 TIMER_CH4  TIM4

 TIMER_TONE TIMER_CH4

 */


typedef struct
{
  TIM_HandleTypeDef hTIM;
  uint8_t  enable;
  uint32_t freq;
  uint32_t prescaler_value;
  uint32_t prescaler_value_1M;
  uint32_t prescaler_div;
  uint32_t period;
  voidFuncPtr handler;
} DRV_TIMER_OBJ;



DRV_TIMER_OBJ hDrvTim[TIMER_CH_MAX];



int drv_timer_init()
{
#if 1
  uint8_t tim_ch;
  uint8_t i;


  //-- TIMER_CH1  TIM1
  //
  tim_ch = TIMER_CH1;
  hDrvTim[tim_ch].hTIM.Instance      = TIM1;
  hDrvTim[tim_ch].prescaler_value    = (uint32_t)((SystemCoreClock / 1) / 10000  ) - 1; // 0.01Mhz
  hDrvTim[tim_ch].prescaler_value_1M = (uint32_t)((SystemCoreClock / 1) / 1000000) - 1; // 1.00Mhz
  hDrvTim[tim_ch].prescaler_div      = 100;
  hDrvTim[tim_ch].hTIM.Init.Period        = 10000 - 1;
  hDrvTim[tim_ch].hTIM.Init.Prescaler     = hDrvTim[tim_ch].prescaler_value;
  hDrvTim[tim_ch].hTIM.Init.ClockDivision = 0;
  hDrvTim[tim_ch].hTIM.Init.CounterMode   = TIM_COUNTERMODE_UP;
  hDrvTim[tim_ch].hTIM.Init.RepetitionCounter = 0;


  //-- TIMER_CH2  TIM2
  //
  tim_ch = TIMER_CH2;
  hDrvTim[tim_ch].hTIM.Instance      = TIM2;
  hDrvTim[tim_ch].prescaler_value    = (uint32_t)((SystemCoreClock / 1) / 10000  ) - 1; // 0.01Mhz
  hDrvTim[tim_ch].prescaler_value_1M = (uint32_t)((SystemCoreClock / 1) / 1000000) - 1; // 1.00Mhz
  hDrvTim[tim_ch].prescaler_div      = 100;
  hDrvTim[tim_ch].hTIM.Init.Period        = 10000 - 1;
  hDrvTim[tim_ch].hTIM.Init.Prescaler     = hDrvTim[tim_ch].prescaler_value;
  hDrvTim[tim_ch].hTIM.Init.ClockDivision = 0;
  hDrvTim[tim_ch].hTIM.Init.CounterMode   = TIM_COUNTERMODE_UP;
  hDrvTim[tim_ch].hTIM.Init.RepetitionCounter = 0;


  //-- TIMER_CH3  TIM3
  //
  tim_ch = TIMER_CH3;
  hDrvTim[tim_ch].hTIM.Instance      = TIM3;
  hDrvTim[tim_ch].prescaler_value    = (uint32_t)((SystemCoreClock / 1) / 10000  ) - 1; // 0.01Mhz
  hDrvTim[tim_ch].prescaler_value_1M = (uint32_t)((SystemCoreClock / 1) / 1000000) - 1; // 1.00Mhz
  hDrvTim[tim_ch].prescaler_div      = 100;
  hDrvTim[tim_ch].hTIM.Init.Period        = 10000 - 1;
  hDrvTim[tim_ch].hTIM.Init.Prescaler     = hDrvTim[tim_ch].prescaler_value;
  hDrvTim[tim_ch].hTIM.Init.ClockDivision = 0;
  hDrvTim[tim_ch].hTIM.Init.CounterMode   = TIM_COUNTERMODE_UP;
  hDrvTim[tim_ch].hTIM.Init.RepetitionCounter = 0;


  //-- TIMER_CH4  TIM4
  //
  tim_ch = TIMER_TONE;
  hDrvTim[tim_ch].hTIM.Instance      = TIM4;
  hDrvTim[tim_ch].prescaler_value    = (uint32_t)((SystemCoreClock / 1) / 10000  ) - 1; // 1Mhz
  hDrvTim[tim_ch].prescaler_value_1M = (uint32_t)((SystemCoreClock / 1) / 1000000) - 1; // 1.00Mhz
  hDrvTim[tim_ch].prescaler_div      = 100;
  hDrvTim[tim_ch].hTIM.Init.Period        = 10000 - 1;
  hDrvTim[tim_ch].hTIM.Init.Prescaler     = hDrvTim[tim_ch].prescaler_value;
  hDrvTim[tim_ch].hTIM.Init.ClockDivision = 0;
  hDrvTim[tim_ch].hTIM.Init.CounterMode   = TIM_COUNTERMODE_UP;
  hDrvTim[tim_ch].hTIM.Init.RepetitionCounter = 0;



  for( i=0; i<TIMER_CH_MAX; i++ )
  {
    hDrvTim[i].handler = NULL;
    hDrvTim[i].enable  = 0;
  }
#endif
  return 0;
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  if( htim->Instance == TIM1 )
  {
    __HAL_RCC_TIM1_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM1_UP_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
  }
  if( htim->Instance == TIM2 )
  {
    __HAL_RCC_TIM2_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  }
  if( htim->Instance == TIM3 )
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM3_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  }
  if( htim->Instance == TIM4 )
  {
    __HAL_RCC_TIM4_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM4_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  }
}


void HAL_TIMEx_CommutationCallback(TIM_HandleTypeDef* htim)
{

}


void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef* htim)
{

}


void TIM1_UP_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&hDrvTim[TIMER_CH1].hTIM);
}
void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&hDrvTim[TIMER_CH2].hTIM);
}
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&hDrvTim[TIMER_CH3].hTIM);
}
void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&hDrvTim[TIMER_CH4].hTIM);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  uint8_t i;


  for( i=0; i<TIMER_CH_MAX; i++ )
  {
    if( htim->Instance == hDrvTim[i].hTIM.Instance )
    {
      if( hDrvTim[i].handler != NULL )
      {
        (*hDrvTim[i].handler)();
      }
    }
  }
}


void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
  if( htim->Instance == TIM1 )
  {
    HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
  }
  if( htim->Instance == TIM2 )
  {
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  }
  if( htim->Instance == TIM3 )
  {
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  }
  if( htim->Instance == TIM4 )
  {
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
  }
}

void drv_timer_pause(uint8_t channel)
{
  if( channel >= TIMER_CH_MAX ) return;

  hDrvTim[channel].enable = 0;
  HAL_TIM_Base_DeInit(&hDrvTim[channel].hTIM);
}

void drv_timer_set_period(uint8_t channel, uint32_t period_data)
{
  if( channel >= TIMER_CH_MAX ) return;

  if( period_data > 0xFFFF )
  {
    hDrvTim[channel].hTIM.Init.Prescaler = hDrvTim[channel].prescaler_value;
    hDrvTim[channel].hTIM.Init.Period    = (period_data/hDrvTim[channel].prescaler_div) - 1;
  }
  else
  {
    if( period_data > 0 )
    {
      hDrvTim[channel].hTIM.Init.Prescaler = hDrvTim[channel].prescaler_value_1M;
      hDrvTim[channel].hTIM.Init.Period    = period_data - 1;
    }
  }
}


void drv_timer_attachInterrupt(uint8_t channel, voidFuncPtr handler)
{
  if( channel >= TIMER_CH_MAX ) return;

  hDrvTim[channel].handler = handler;
}


void drv_timer_detachInterrupt(uint8_t channel)
{
  if( channel >= TIMER_CH_MAX ) return;

  hDrvTim[channel].handler = NULL;
}

void drv_timer_refresh(uint8_t channel)
{

}


void drv_timer_resume(uint8_t channel)
{
  if( channel >= TIMER_CH_MAX ) return;

  HAL_TIM_Base_Init(&hDrvTim[channel].hTIM);
  HAL_TIM_Base_Start_IT(&hDrvTim[channel].hTIM);

  hDrvTim[channel].enable = 1;
}
