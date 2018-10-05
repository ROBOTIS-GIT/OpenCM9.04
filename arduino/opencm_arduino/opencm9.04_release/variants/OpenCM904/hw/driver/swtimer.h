/*
 * swtimer.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef SWTIMER_H_
#define SWTIMER_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "def.h"
#include "bsp.h"






#ifndef NULL
#define NULL 0
#endif


#ifndef _HW_DEF_SW_TIMER_MAX
#define _HW_DEF_SW_TIMER_MAX        8
#endif


#define ONE_TIME            1
#define LOOP_TIME           2



typedef int16_t             swtimer_handle_t;



typedef struct
{

  uint8_t  Timer_En;             // 타이머 인에이블 신호
  uint8_t  Timer_Mode;           // 타이머 모드
  uint32_t Timer_Ctn;            // 현제의 타이머 값
  uint32_t Timer_Init;           // 타이머 초기화될때의 카운트 값
  void (*TmrFnct)(void *);       // 만료될때 실행될 함수
  void *TmrFnctArg;              // 함수로 전달할 인수들
} swtimer_t;



bool swtimerInit(void);
void swtimerSet  (uint8_t TmrNum, uint32_t TmrData, uint8_t TmrMode, void (*Fnct)(void *),void *arg);
void swTimerSetCnt (uint8_t TmrNum, uint32_t TmrCnt);
void swtimerStart(uint8_t TmrNum);
void swtimerStop (uint8_t TmrNum);
void swtimerReset(uint8_t TmrNum);
void swtimerISR(void);


int16_t  swtimerGetHandle(void);
uint32_t swtimerGetCounter(void);
uint32_t swtimerGetMicroCounter(void);



#ifdef __cplusplus
}
#endif

#endif /* PRJ_SDK_HAL_LED_H_ */
