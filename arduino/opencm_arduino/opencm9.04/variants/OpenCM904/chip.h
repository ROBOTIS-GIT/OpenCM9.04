#ifndef _CHIP_OPENCM904_
#define _CHIP_OPENCM904_

#include <stdbool.h>
#include "bsp.h"
#include "hw.h"







#define USE_SPI1
#define USE_SPI2


#define BOARD_NR_I2C  			    2
#define HAL_I2C1      			    I2C1
#define HAL_I2C2      			    I2C2
#define HAL_I2C3      			    I2C3




#define PIN_SPI_SS              22
#define PIN_SPI_MOSI            21
#define PIN_SPI_MISO            20
#define PIN_SPI_SCK             19


static const uint8_t SS   =     PIN_SPI_SS;
static const uint8_t MOSI =     PIN_SPI_MOSI;
static const uint8_t MISO =     PIN_SPI_MISO;
static const uint8_t SCK  =     PIN_SPI_SCK;





#define BOARD_NR_ADC_PINS       10
#define BOARD_NR_PWM_PINS       12

#define LED_BUILTIN             14



#define BOARD_BUTTON_PIN        23  //PC15
#define BOARD_LED_PIN           14  //PB9



#define BOARD_NR_GPIO_PINS      26


#define BOARD_USART1_TX_PIN     11  //D9(PA9)
#define BOARD_USART1_RX_PIN     12  //D10(PA10)
#define BOARD_USART2_TX_PIN     4   //D2 (PA2)
#define BOARD_USART2_RX_PIN     5   //D3 (PA3)
#define BOARD_USART3_TX_PIN     24  //D24 (PB10)
#define BOARD_USART3_RX_PIN     25  //D25 (PB11)



#define BOARD_SPI1_NSS_PIN      0   //D10 (PA4)
#define BOARD_SPI1_MOSI_PIN     7   //D11 PA7
#define BOARD_SPI1_MISO_PIN     6   //D12 PA6
#define BOARD_SPI1_SCK_PIN      1   //D13 PA5
#define BOARD_SPI2_NSS_PIN      18  //D26 PB12
#define BOARD_SPI2_MOSI_PIN     21  //D29 PB15
#define BOARD_SPI2_MISO_PIN     20  //D28 PB14
#define BOARD_SPI2_SCK_PIN      19  //D27 PB13


#endif
