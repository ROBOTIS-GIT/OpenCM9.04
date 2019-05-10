/*
 * swtimer.c
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */



#include <stdarg.h>
#include <stdbool.h>

#include "swtimer.h"
#include "timer.h"




//-- Internal Variables
//
static volatile uint32_t sw_timer_counter      = 0;
static volatile uint16_t sw_timer_handle_index = 0;
static volatile uint32_t sw_timer_mcros        = 0;
static volatile uint32_t sw_timer_mcros_h      = 0;
static volatile uint32_t sw_timer_mcros_l      = 0;

static swtimer_t  swtimer_tbl[_HW_DEF_SW_TIMER_MAX];           // 타이머 배열 선언





//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//





bool swtimerInit(void)
{
  uint8_t i;
  static uint8_t excute = 0;


  if (excute == 1) return false;  // 이미 한번 실행했다면 정지.


  // 구조체 초기화
  for(i=0; i<_HW_DEF_SW_TIMER_MAX; i++)
  {
    swtimer_tbl[i].Timer_En   = false;
    swtimer_tbl[i].Timer_Ctn  = 0;
    swtimer_tbl[i].Timer_Init = 0;
    swtimer_tbl[i].TmrFnct    = NULL;
  }

  excute = 1;

  return true;
}

void swtimerISR(void)
{
  uint8_t i;


  sw_timer_counter++;
  sw_timer_mcros += 1000;


  for (i=0; i<_HW_DEF_SW_TIMER_MAX && i<sw_timer_handle_index; i++)     // 타이머 갯수만큼
  {
    if ( swtimer_tbl[i].Timer_En == true)                       // 타이머가 활성화 됬니?
    {
      swtimer_tbl[i].Timer_Ctn--;                               // 타이머값 감소

      if (swtimer_tbl[i].Timer_Ctn == 0)                        // 타이머 오버플로어
      {
        if(swtimer_tbl[i].Timer_Mode == ONE_TIME)               // 한번만 실행하는거면
        {
          swtimer_tbl[i].Timer_En = false;                      // 타이머 OFF 한다.
        }

        swtimer_tbl[i].Timer_Ctn = swtimer_tbl[i].Timer_Init;   // 타이머 초기화

        (*swtimer_tbl[i].TmrFnct)(swtimer_tbl[i].TmrFnctArg);   // 전달변수 없이 함수 실행
      }
    }
  }

}

void swtimerSet(uint8_t TmrNum, uint32_t TmrData, uint8_t TmrMode, void (*Fnct)(void *),void *arg)
{
  swtimer_tbl[TmrNum].Timer_Mode = TmrMode;    // 모트설정
  swtimer_tbl[TmrNum].TmrFnct    = Fnct;       // 실행할 함수
  swtimer_tbl[TmrNum].TmrFnctArg = arg;        // 매개변수
  swtimer_tbl[TmrNum].Timer_Ctn  = TmrData;
  swtimer_tbl[TmrNum].Timer_Init = TmrData;
}

void swTimerSetCnt (uint8_t TmrNum, uint32_t TmrCnt) 
{
  if(TmrNum < _HW_DEF_SW_TIMER_MAX)
  {
    swtimer_tbl[TmrNum].Timer_Ctn = TmrCnt; 
  }
}

void swtimerStart(uint8_t TmrNum)
{
  if(TmrNum < _HW_DEF_SW_TIMER_MAX)
  {
    swtimer_tbl[TmrNum].Timer_En = true;
  }
}

void swtimerStop (uint8_t TmrNum)
{
  if(TmrNum < _HW_DEF_SW_TIMER_MAX)
  {
    swtimer_tbl[TmrNum].Timer_En = false;
  }
}

void swtimerReset(uint8_t TmrNum)
{
  swtimer_tbl[TmrNum].Timer_En   = false;
  swtimer_tbl[TmrNum].Timer_Ctn  = swtimer_tbl[TmrNum].Timer_Init;
}

swtimer_handle_t swtimerGetHandle(void)
{
  swtimer_handle_t TmrIndex = sw_timer_handle_index;

  sw_timer_handle_index++;

  return TmrIndex;
}

uint32_t swtimerGetCounter(void)
{
  return sw_timer_counter;
}

uint32_t swtimerGetMicroCounter(void)
{
  uint32_t micro_counter;
  uint32_t cnt_h;
  uint32_t cnt_l;


  cnt_h   = sw_timer_mcros;
  cnt_l   = timerGetMicros();

  if (sw_timer_mcros_h == cnt_h)
  {
    if (sw_timer_mcros_l > cnt_l)
    {
      micro_counter = cnt_h + cnt_l + 1000;
    }
    else
    {
      micro_counter = cnt_h + cnt_l;
    }
  }
  else
  {
    micro_counter = cnt_h + cnt_l;
  }


  sw_timer_mcros_h = cnt_h;
  sw_timer_mcros_l = cnt_l;

  return micro_counter;
}
