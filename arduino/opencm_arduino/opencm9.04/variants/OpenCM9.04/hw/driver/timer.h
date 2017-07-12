/*
 * timer.h
 *
 *  Created on: 2017. 2. 13.
 *      Author: baram
 */

#ifndef TIMER_H_
#define TIMER_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "def.h"
#include "bsp.h"








bool timerInit(void);


uint32_t timerGetMicros(void);




#ifdef __cplusplus
}
#endif

#endif /* TIMER_H_ */
