/*
 *  drv_pwm.c
 *
 *  Created on: 2016. 7. 14.
 *      Author: Baram, PBHP
 */

#include "drv_pwm.h"
#include "variant.h"


#define PWM_PERIOD_VALUE  255


TIM_HandleTypeDef         hTIM1;
TIM_HandleTypeDef         hTIM2;
TIM_HandleTypeDef         hTIM3;
TIM_HandleTypeDef         hTIM4;

TIM_OC_InitTypeDef        hOC1[4];
TIM_OC_InitTypeDef        hOC2[4];
TIM_OC_InitTypeDef        hOC3[4];
TIM_OC_InitTypeDef        hOC4[4];




uint32_t pwm_freq[PINS_COUNT];
bool     pwm_init[PINS_COUNT];
uint32_t pwm_period[PINS_COUNT];




static TIM_OC_InitTypeDef *drv_pwm_get_oc(uint32_t ulPin);
static void drv_pwm_setup_gpio(uint32_t ulPin);


int drv_pwm_init()
{
  uint32_t i;


  for( i=0; i<PINS_COUNT; i++ )
  {
    pwm_init[i]     = false;
    pwm_period[i]   = PWM_PERIOD_VALUE;
    pwm_freq[i]     = 50000;              // 50Khz
  }
  return 0;
}


static long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint8_t drv_pwm_get_init(uint32_t ulPin)
{
  return pwm_init[ulPin];
}

void drv_pwm_pin_release(uint32_t ulPin)
{
  if (pwm_init[ulPin] == true)
  {
    pwm_init[ulPin] = false;
    HAL_GPIO_DeInit(g_Pin2PortMapArray[ulPin].GPIOx_Port, g_Pin2PortMapArray[ulPin].Pin_abstraction);
  }
}

void drv_pwm_setup_freq(uint32_t ulPin, uint32_t pwm_freq)
{
  uint32_t timer_freq;
  uint32_t timer_period;

  timer_freq   = SystemCoreClock;
  timer_period = (uint32_t) (((SystemCoreClock)  / pwm_freq - 1));

  drv_pwm_setup(ulPin, timer_freq, timer_period);
}

void drv_pwm_setup(uint32_t ulPin, uint32_t freq, uint32_t period)
{
  TIM_HandleTypeDef  *pTIM;
  TIM_OC_InitTypeDef *pOC;
  uint32_t tim_ch;
  uint32_t uwPeriodValue;
  uint32_t prescaler_value;
  uint32_t pwm_freq_value;


  if( ulPin >= PINS_COUNT )     return;
  if( pwm_init[ulPin] == true ) return;
  if( g_Pin2PortMapArray[ulPin].timerChannel == NO_PWM ) return;


  pTIM   = g_Pin2PortMapArray[ulPin].TIMx;
  tim_ch = g_Pin2PortMapArray[ulPin].timerChannel;


  pwm_freq[ulPin]   = freq;
  pwm_period[ulPin] = period;

  prescaler_value = (uint32_t) (((SystemCoreClock/1)  / freq) - 1);

  pOC = drv_pwm_get_oc(ulPin);

  if( pTIM == &hTIM1 )
  {
    pTIM->Instance = TIM1;
  }
  else if( pTIM == &hTIM2 )
  {
    pTIM->Instance = TIM2;
  }
  else if( pTIM == &hTIM3 )
  {
    pTIM->Instance = TIM3;
  }
  else if( pTIM == &hTIM4 )
  {
    pTIM->Instance = TIM4;
  }
  else
  {
    return;
  }


  if (HAL_TIM_PWM_GetState(pTIM) != HAL_TIM_STATE_READY)
  {
    pTIM->Init.Prescaler         = prescaler_value;
    pTIM->Init.Period            = period-1;
    pTIM->Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    pTIM->Init.CounterMode       = TIM_COUNTERMODE_UP;
    pTIM->Init.RepetitionCounter = 0;
    HAL_TIM_PWM_Init(pTIM);
  }

  memset(pOC, 0, sizeof(TIM_OC_InitTypeDef));

  pOC->OCMode       = TIM_OCMODE_PWM1;
  pOC->OCPolarity   = TIM_OCPOLARITY_HIGH;
  pOC->OCFastMode   = TIM_OCFAST_DISABLE;
  pOC->OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  pOC->OCNIdleState = TIM_OCNIDLESTATE_RESET;
  pOC->OCIdleState  = TIM_OCIDLESTATE_RESET;

  pOC->Pulse = 0;
  HAL_TIM_PWM_ConfigChannel(pTIM, pOC, tim_ch);
  HAL_TIM_PWM_Start(pTIM, tim_ch);
  pwm_init[ulPin] = true;

  drv_pwm_setup_gpio(ulPin);
}

void drv_pwm_set_duty(uint32_t ulPin, uint32_t res, uint32_t ulDuty )
{
  uint32_t pwm_out;



  ulDuty  = constrain(ulDuty, 0, (1<<res)-1);
  pwm_out = map( ulDuty, 0, (1<<res)-1, 0, pwm_period[ulPin] );


  drv_pwm_set_data(ulPin, pwm_out);
}

void drv_pwm_set_data(uint32_t ulPin, uint32_t pwm_data )
{
  TIM_HandleTypeDef  *pTIM;
  TIM_OC_InitTypeDef *pOC;
  uint32_t tim_ch;
  uint32_t pwm_out;

  if( ulPin >= PINS_COUNT )     return;
  if( pwm_init[ulPin] == false ) return;


  pTIM   = g_Pin2PortMapArray[ulPin].TIMx;
  tim_ch = g_Pin2PortMapArray[ulPin].timerChannel;

  pOC = drv_pwm_get_oc(ulPin);


  pwm_out = constrain(pwm_data, 0, pwm_period[ulPin]);


  if (pOC->Pulse != pwm_out)
  {
    pOC->Pulse = pwm_out;
    HAL_TIM_PWM_ConfigChannel(pTIM, pOC, tim_ch);
    HAL_TIM_PWM_Start(pTIM, tim_ch);
  }
}


void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;


  if( htim->Instance == TIM1 )
  {
    __HAL_RCC_TIM1_CLK_ENABLE();
  }
  if( htim->Instance == TIM2 )
  {
    __HAL_RCC_TIM2_CLK_ENABLE();
  }
  if( htim->Instance == TIM3 )
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
  if( htim->Instance == TIM4 )
  {
    __HAL_RCC_TIM4_CLK_ENABLE();
  }
}

void drv_pwm_setup_gpio(uint32_t ulPin)
{
  GPIO_InitTypeDef   GPIO_InitStruct;


  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pin       = g_Pin2PortMapArray[ulPin].Pin_abstraction;

  HAL_GPIO_DeInit(g_Pin2PortMapArray[ulPin].GPIOx_Port, g_Pin2PortMapArray[ulPin].Pin_abstraction);
  HAL_GPIO_Init(g_Pin2PortMapArray[ulPin].GPIOx_Port, &GPIO_InitStruct);
}

TIM_OC_InitTypeDef *drv_pwm_get_oc(uint32_t ulPin)
{
  TIM_HandleTypeDef  *pTIM;
  TIM_OC_InitTypeDef *pOC;
  uint32_t tim_ch = 0;


  pTIM   = g_Pin2PortMapArray[ulPin].TIMx;


  switch(g_Pin2PortMapArray[ulPin].timerChannel)
  {
    case TIM_CHANNEL_1:
      tim_ch = 0;
      break;

    case TIM_CHANNEL_2:
      tim_ch = 1;
      break;

    case TIM_CHANNEL_3:
      tim_ch = 2;
      break;

    case TIM_CHANNEL_4:
      tim_ch = 3;
      break;
  }

  if( pTIM == &hTIM1 )
  {
    pOC = hOC1;
  }
  else if( pTIM == &hTIM2 )
  {
    pOC = hOC2;
  }
  else if( pTIM == &hTIM3 )
  {
    pOC = hOC3;
  }
  else if( pTIM == &hTIM4 )
  {
    pOC = hOC4;
  }
  else
  {
    pOC = hOC1;
  }

  return &pOC[tim_ch];
}
