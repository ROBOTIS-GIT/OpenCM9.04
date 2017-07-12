/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
 /*
 *  Modified on: 2016. 7.12.
 *       Author: Baram, PBHP
 */
#include "Arduino.h"




#ifdef __cplusplus
extern "C" {
#endif




extern const Pin2PortMapArray g_Pin2PortMapArray[]=
{
    {GPIOA, GPIO_PIN_4,   &hADC1,   ADC_CHANNEL_4 , NULL   ,   NO_PWM       , 0       },  // 0
    {GPIOA, GPIO_PIN_5,   &hADC1,   ADC_CHANNEL_5 , NULL   ,   NO_PWM       , 1       },  // 1
    {GPIOA, GPIO_PIN_0,   &hADC1,   ADC_CHANNEL_0 , &hTIM2 ,   TIM_CHANNEL_1, 2       },  // 2
    {GPIOA, GPIO_PIN_1,   &hADC1,   ADC_CHANNEL_1 , &hTIM2 ,   TIM_CHANNEL_2, 3       },  // 3
    {GPIOA, GPIO_PIN_2,   &hADC1,   ADC_CHANNEL_2 , &hTIM2 ,   TIM_CHANNEL_3, 4       },  // 4
    {GPIOA, GPIO_PIN_3,   &hADC1,   ADC_CHANNEL_3 , &hTIM2 ,   TIM_CHANNEL_4, 5       },  // 5
    {GPIOA, GPIO_PIN_6,   &hADC1,   ADC_CHANNEL_6 , &hTIM3 ,   TIM_CHANNEL_1, 6       },  // 6
    {GPIOA, GPIO_PIN_7,   &hADC1,   ADC_CHANNEL_7 , &hTIM3 ,   TIM_CHANNEL_2, 7       },  // 7
    {GPIOB, GPIO_PIN_0,   &hADC1,   ADC_CHANNEL_8 , &hTIM3 ,   TIM_CHANNEL_3, 8       },  // 8
    {GPIOB, GPIO_PIN_1,   &hADC1,   ADC_CHANNEL_9 , &hTIM3 ,   TIM_CHANNEL_4, 9       },  // 9

    {GPIOA, GPIO_PIN_8,   NULL,     NO_ADC        , &hTIM1 ,   TIM_CHANNEL_1, 10      },  // 10
    {GPIOA, GPIO_PIN_9,   NULL,     NO_ADC        , &hTIM1 ,   TIM_CHANNEL_2, 11      },  // 11
    {GPIOA, GPIO_PIN_10,  NULL,     NO_ADC        , &hTIM1 ,   TIM_CHANNEL_3, 12      },  // 12
    {GPIOB, GPIO_PIN_8,   NULL,     NO_ADC        , &hTIM4 ,   TIM_CHANNEL_3, 13      },  // 13
    {GPIOB, GPIO_PIN_9,   NULL,     NO_ADC        , &hTIM4 ,   TIM_CHANNEL_4, 14      },  // 14 LED
    {GPIOA, GPIO_PIN_15,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 15      },  // 15
    {GPIOB, GPIO_PIN_3,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , 16      },  // 16
    {GPIOB, GPIO_PIN_4,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , 17      },  // 17
    {GPIOB, GPIO_PIN_12,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 18      },  // 18
    {GPIOB, GPIO_PIN_13,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 19      },  // 19

    {GPIOB, GPIO_PIN_14,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 20      },  // 20
    {GPIOB, GPIO_PIN_15,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 21      },  // 21
    {GPIOC, GPIO_PIN_14,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 22      },  // 22
    {GPIOC, GPIO_PIN_15,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 23      },  // 23 USER_BUTTON
    {GPIOB, GPIO_PIN_10,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 24      },  // 24
    {GPIOB, GPIO_PIN_11,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 25      },  // 25


    {GPIOA, GPIO_PIN_13,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 26 JTAG SWDIO
    {GPIOA, GPIO_PIN_14,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 27 JTAG SWDCLK
    {GPIOB, GPIO_PIN_5,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 28 DXL DIR
    {GPIOB, GPIO_PIN_6,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 29 DXL TXD
    {GPIOB, GPIO_PIN_7,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 30 DXL RXD

    {NULL , 0          ,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI }
};


#ifdef __cplusplus
}
#endif




/* ----------------------------------------------------------------------------
 *     USART objects
 * ----------------------------------------------------------------------------*/
void serialEvent() __attribute__((weak));
void serialEvent() { }

void serialEvent1() __attribute__((weak));
void serialEvent1() { }

void serialEvent2() __attribute__((weak));
void serialEvent2() { }

void serialEvent3() __attribute__((weak));
void serialEvent3() { }


USBSerial Serial;
UARTClass Serial1(DRV_UART_NUM_1, DRV_UART_DMA_MODE);
UARTClass Serial2(DRV_UART_NUM_2, DRV_UART_DMA_MODE);
UARTClass Serial3(DRV_UART_NUM_3, DRV_UART_DMA_MODE);



void serialEventRun(void)
{
  if (Serial.available()) serialEvent();
  if (Serial1.available()) serialEvent1();
  if (Serial2.available()) serialEvent2();
  if (Serial3.available()) serialEvent3();
}


void var_init(void)
{

}

