/*
 * timer.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */



#include <stdarg.h>
#include <stdbool.h>

#include "timer.h"





//-- Internal Variables
//



//-- External Variables
//


//-- Internal Functions
//
void timerInitTimer3(void);


//-- External Functions
//



bool timerInit(void)
{
  return true;
}


uint32_t timerGetMicros(void)
{
  uint32_t count_val;


  count_val = SysTick->LOAD - SysTick->VAL;
  return (count_val) / 72;
}


