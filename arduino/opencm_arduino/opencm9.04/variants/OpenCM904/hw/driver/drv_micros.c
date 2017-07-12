/*
 *  drv.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram, PBHP
 */

#include "drv_micros.h"
#include "swtimer.h"
#include "variant.h"

#define US_PER_MS               1000
#define CYCLES_PER_MICROSECOND  72
#define SYSTICK_RELOAD_VAL      (CYCLES_PER_MICROSECOND*1000-1)



TIM_HandleTypeDef    TimHandle;






void drv_micros_init()
{
#if 0
  uint32_t uwPrescalerValue = 0;


  __HAL_RCC_TIM5_CLK_ENABLE();


  // Compute the prescaler value to have TIMx counter clock equal to 1Mh
  uwPrescalerValue = (uint32_t)((SystemCoreClock / 2) / 1000000) - 1;


  TimHandle.Instance = TIM5;
  TimHandle.Init.Period            = 0xFFFFFFFF;
  TimHandle.Init.Prescaler         = uwPrescalerValue;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;


  HAL_TIM_Base_Init(&TimHandle);
  HAL_TIM_Base_Start(&TimHandle);
#endif
}


uint32_t drv_micros()
{
#if 1
  return swtimerGetMicroCounter();
#else
  uint32_t ms;
  uint32_t cycle_cnt;
  uint32_t res;

  do {
      ms = millis();
      cycle_cnt = systick_get_count();
      asm volatile("nop"); //allow interrupt to fire
      asm volatile("nop");
  } while (ms != millis());

  if(systick_check_underflow()) {
    ms++;
    cycle_cnt = systick_get_count();
  }

  /* SYSTICK_RELOAD_VAL is 1 less than the number of cycles it
     actually takes to complete a SysTick reload */
  res = (ms * US_PER_MS) +
      (SYSTICK_RELOAD_VAL + 1 - cycle_cnt) / CYCLES_PER_MICROSECOND;

  return res;
#endif
}



