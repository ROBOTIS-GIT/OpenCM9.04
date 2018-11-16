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
    {GPIOA, GPIO_PIN_5,   &hADC1,   ADC_CHANNEL_5 , NULL   ,   NO_PWM       , 1       },  // 1                  SPI1_SCK
    {GPIOA, GPIO_PIN_0,   &hADC1,   ADC_CHANNEL_0 , &hTIM2 ,   TIM_CHANNEL_1, 2       },  // 2
    {GPIOA, GPIO_PIN_1,   &hADC1,   ADC_CHANNEL_1 , &hTIM2 ,   TIM_CHANNEL_2, 3       },  // 3
    {GPIOA, GPIO_PIN_2,   &hADC1,   ADC_CHANNEL_2 , &hTIM2 ,   TIM_CHANNEL_3, 4       },  // 4                              UART1_TXD
    {GPIOA, GPIO_PIN_3,   &hADC1,   ADC_CHANNEL_3 , &hTIM2 ,   TIM_CHANNEL_4, 5       },  // 5                              UART1_RXD
    {GPIOA, GPIO_PIN_6,   &hADC1,   ADC_CHANNEL_6 , &hTIM3 ,   TIM_CHANNEL_1, 6       },  // 6                  SPI1_MISO
    {GPIOA, GPIO_PIN_7,   &hADC1,   ADC_CHANNEL_7 , &hTIM3 ,   TIM_CHANNEL_2, 7       },  // 7                  SPI1_MOSI
    {GPIOB, GPIO_PIN_0,   &hADC1,   ADC_CHANNEL_8 , &hTIM3 ,   TIM_CHANNEL_3, 8       },  // 8
    {GPIOB, GPIO_PIN_1,   &hADC1,   ADC_CHANNEL_9 , &hTIM3 ,   TIM_CHANNEL_4, 9       },  // 9

    {GPIOA, GPIO_PIN_8,   NULL,     NO_ADC        , &hTIM1 ,   TIM_CHANNEL_1, 10      },  // 10
    {GPIOA, GPIO_PIN_9,   NULL,     NO_ADC        , &hTIM1 ,   TIM_CHANNEL_2, 11      },  // 11                             UART2_TXD
    {GPIOA, GPIO_PIN_10,  NULL,     NO_ADC        , &hTIM1 ,   TIM_CHANNEL_3, 12      },  // 12                             UART2_RXD
    {GPIOB, GPIO_PIN_8,   NULL,     NO_ADC        , &hTIM4 ,   TIM_CHANNEL_3, 13      },  // 13                 I2C1 SCL
    {GPIOB, GPIO_PIN_9,   NULL,     NO_ADC        , &hTIM4 ,   TIM_CHANNEL_4, 14      },  // 14 LED             I2C1 SDA
    {GPIOA, GPIO_PIN_15,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 15      },  // 15
    {GPIOB, GPIO_PIN_3,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , 16      },  // 16
    {GPIOB, GPIO_PIN_4,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , 17      },  // 17
    {GPIOB, GPIO_PIN_12,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 18      },  // 18
    {GPIOB, GPIO_PIN_13,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 19      },  // 19                 SPI2_SCK

    {GPIOB, GPIO_PIN_14,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 20      },  // 20                 SPI2_MISO
    {GPIOB, GPIO_PIN_15,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 21      },  // 21                 SPI2_MOSI
    {GPIOC, GPIO_PIN_14,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 22      },  // 22
    {GPIOC, GPIO_PIN_15,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 23      },  // 23 USER_BUTTON
    {GPIOB, GPIO_PIN_10,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 24      },  // 24                 I2C2 SCL    UART3_TXD
    {GPIOB, GPIO_PIN_11,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , 25      },  // 25                 I2C2 SCL    UART3_RXD


    {GPIOA, GPIO_PIN_13,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 26 JTAG SWDIO
    {GPIOA, GPIO_PIN_14,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 27 JTAG SWDCLK
    {GPIOB, GPIO_PIN_5,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 28 DXL DIR
    {GPIOB, GPIO_PIN_6,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 29 DXL TXD
    {GPIOB, GPIO_PIN_7,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 30 DXL RXD

    {NULL , 0          ,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI }
};

const uint32_t *digital_pin_bitband_table[]
{
    GPIO_BITBAND_PTR(GPIOA->ODR, 4),  // 0
    GPIO_BITBAND_PTR(GPIOA->ODR, 5),  // 1                  SPI1_SCK
    GPIO_BITBAND_PTR(GPIOA->ODR, 0),  // 2
    GPIO_BITBAND_PTR(GPIOA->ODR, 1),  // 3
    GPIO_BITBAND_PTR(GPIOA->ODR, 2),  // 4                              UART1_TXD
    GPIO_BITBAND_PTR(GPIOA->ODR, 3),  // 5                              UART1_RXD
    GPIO_BITBAND_PTR(GPIOA->ODR, 6),  // 6                  SPI1_MISO
    GPIO_BITBAND_PTR(GPIOA->ODR, 7),  // 7                  SPI1_MOSI
    GPIO_BITBAND_PTR(GPIOB->ODR, 0),  // 8
    GPIO_BITBAND_PTR(GPIOB->ODR, 1),  // 9
    GPIO_BITBAND_PTR(GPIOA->ODR, 8),  // 10
    GPIO_BITBAND_PTR(GPIOA->ODR, 9),  // 11                             UART2_TXD
    GPIO_BITBAND_PTR(GPIOA->ODR, 10),  // 12                             UART2_RXD
    GPIO_BITBAND_PTR(GPIOB->ODR, 8),  // 13                 I2C1 SCL
    GPIO_BITBAND_PTR(GPIOB->ODR, 9),  // 14 LED             I2C1 SDA
    GPIO_BITBAND_PTR(GPIOA->ODR, 15),  // 15
    GPIO_BITBAND_PTR(GPIOB->ODR, 3),  // 16
    GPIO_BITBAND_PTR(GPIOB->ODR, 4),  // 17
    GPIO_BITBAND_PTR(GPIOB->ODR, 12),  // 18
    GPIO_BITBAND_PTR(GPIOB->ODR, 13),  // 19                 SPI2_SCK
    GPIO_BITBAND_PTR(GPIOB->ODR, 14),  // 20                 SPI2_MISO
    GPIO_BITBAND_PTR(GPIOB->ODR, 15),  // 21                 SPI2_MOSI
    GPIO_BITBAND_PTR(GPIOC->ODR, 14),  // 22
    GPIO_BITBAND_PTR(GPIOC->ODR, 15),  // 23 USER_BUTTON
    GPIO_BITBAND_PTR(GPIOB->ODR, 10),  // 24                 I2C2 SCL    UART3_TXD
    GPIO_BITBAND_PTR(GPIOB->ODR, 11),  // 25                 I2C2 SCL    UART3_RXD
    GPIO_BITBAND_PTR(GPIOA->ODR, 13),  // 26 JTAG SWDIO
    GPIO_BITBAND_PTR(GPIOA->ODR, 14),  // 27 JTAG SWDCLK
    GPIO_BITBAND_PTR(GPIOB->ODR, 5),  // 28 DXL DIR
    GPIO_BITBAND_PTR(GPIOB->ODR, 6),  // 29 DXL TXD
    GPIO_BITBAND_PTR(GPIOB->ODR, 7)  // 30 DXL RXD
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

uint8_t serial1_tx_buffer[SERIAL_BUFFER_SIZE];
uint8_t serial2_tx_buffer[SERIAL_BUFFER_SIZE];
uint8_t serial3_tx_buffer[SERIAL_BUFFER_SIZE];

UARTClass Serial1(DRV_UART_NUM_1, DRV_UART_DMA_MODE, serial1_tx_buffer, sizeof(serial1_tx_buffer));
UARTClass Serial2(DRV_UART_NUM_2, DRV_UART_DMA_MODE, serial2_tx_buffer, sizeof(serial2_tx_buffer));
UARTClass Serial3(DRV_UART_NUM_3, DRV_UART_DMA_MODE, serial3_tx_buffer, sizeof(serial3_tx_buffer));

void Tx1_Handler(void){ Serial1.TxHandler(); }
void Tx2_Handler(void){ Serial2.TxHandler(); }
void Tx3_Handler(void){ Serial3.TxHandler(); }



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

void toggleLED(void)
{
  digitalWrite(BOARD_LED_PIN, !digitalRead(BOARD_LED_PIN));
}
