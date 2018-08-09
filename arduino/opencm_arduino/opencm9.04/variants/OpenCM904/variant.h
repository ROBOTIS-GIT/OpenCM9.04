
#ifndef _VARIANT_OPENCM904_
#define _VARIANT_OPENCM904_

#include <chip.h>



#define NO_ADC 		0xffff
#define NO_PWM		0xffff
#define NO_EXTI   0xffff


/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "Arduino.h"
#include "hw.h"


#ifdef __cplusplus
#include "UARTClass.h"
#include "USBSerial.h"
#include "HardwareTimer.h"
#endif


#ifdef __cplusplus
extern "C"{
#endif // __cplusplus



//-- Analog pins
//
static const uint8_t A0  = 0;
static const uint8_t A1  = 1;
static const uint8_t A2  = 2;
static const uint8_t A3  = 3;
static const uint8_t A4  = 4;
static const uint8_t A5  = 5;
static const uint8_t A6  = 6;
static const uint8_t A7  = 7;
static const uint8_t A8  = 8;
static const uint8_t A9  = 9;


//-- Digital pins
//
enum
{
  D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15, D16,
  D17, D18, D19, D20, D21, D22, D23, D24, D25, D26, D27, D28, D29, D30, D31,
  D32, D33, D34, D35, D36, D37, D38, D39, D40, D41, D42, D43, D44, D45, D46,
  D47, D48, D49, D50, D51, D52, D53, D54, D55, D56, D57, D58, D59, D60, D61,
  D62, D63, D64, D65, D66, D67, D68, D69, D70, D71, D72, D73, D74, D75, D76,
  D77, D78, D79, D80, D81, D82, D83, D84, D85, D86, D87, D88, D89, D90, D91,
  D92, D93, D94, D95, D96, D97, D98, D99, D100
};


enum
{
  PIN_CONFIG_NONE,
  PIN_CONFIG_INPUT,
  PIN_CONFIG_OUTPUT,
  PIN_CONFIG_ADC,
  PIN_CONFIG_PWM,
  PIN_CONFIG_EXTI
};


typedef struct _Pin2PortMapArray
{
  	GPIO_TypeDef      *GPIOx_Port;

  	uint32_t 	         Pin_abstraction;

  	ADC_HandleTypeDef *ADCx;
    uint32_t           adc_channel;

    TIM_HandleTypeDef *TIMx;
    uint32_t           timerChannel;
    uint32_t           extiChannel;
} Pin2PortMapArray ;


extern const Pin2PortMapArray g_Pin2PortMapArray[];

// Arm M3 standard bit band definitions. 
#define GPIO_BITBAND_ADDR(reg, bit) (((uint32_t)&(reg) - 0x40000000) * 32 + (bit) * 4 + 0x42000000)
#define GPIO_BITBAND_PTR(reg, bit) ((uint32_t *)GPIO_BITBAND_ADDR((reg), (bit)))

extern const uint32_t *digital_pin_bitband_table[];
// compatibility macros
#define digitalPinToPort(pin) (pin)
#define digitalPinToBitMask(pin) (1)
#define portOutputRegister(pin) ((volatile uint32_t *)(digital_pin_bitband_table[(pin)] + 0))    // We point to ODR so 0 offset
#define portInputRegister(pin)  ((volatile uint32_t *)(digital_pin_bitband_table[(pin)] - 32))  // ODR offset(0xc) IDR Offset(0x8) so offset -4*32

#define portSetRegister(pin)    ((volatile uint32_t *)(digital_pin_bitband_table[(pin)] + 32))  // BSRR(0x10) so offset +4*32
#define portClearRegister(pin)  ((volatile uint32_t *)(digital_pin_bitband_table[(pin)] + 64))  // brr(0X14) so offset +8*32 or could also use BSRR
#define digitalPinToPortReg(pin) (portOutputRegister(pin))
#define digitalPinToBit(pin) (1)


void Rx1_Handler(void);
void Tx1_Handler(void);
void Rx2_Handler(void);
void Tx2_Handler(void);
void Rx3_Handler(void);
void Tx3_Handler(void);

void Err1_Handler(void);
void Err2_Handler(void);
void Err3_Handler(void);


#ifdef __cplusplus
}
#endif


/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus

extern USBSerial Serial;    // USB
extern UARTClass Serial1;   // Serial for UART1
extern UARTClass Serial2;   // Serial for UART2
extern UARTClass Serial3;   // Serial for UART3


#define SerialUSB Serial



#endif


#define digitalPinToInterrupt(P)   ( g_Pin2PortMapArray[P].extiChannel )
#define analogPinToChannel(p)      ( (p) < 10 ? (p)+A0 : (p) )


void  var_init();
void  toggleLED(void);

#define WIRE_INTERFACES_COUNT       1
#define SPI_INTERFACES_COUNT        2
#define DXL_COUNT                   3
#define EXTI_COUNT                  26
#define PINS_COUNT                  30

#endif
