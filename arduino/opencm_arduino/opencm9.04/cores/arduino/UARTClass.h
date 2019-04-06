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

#ifndef _UART_CLASS_
#define _UART_CLASS_

#include "HardwareSerial.h"
#include <chip.h>


#define SERIAL_8N1 UARTClass::Mode_8N1
#define SERIAL_8E1 UARTClass::Mode_8E1
#define SERIAL_8O1 UARTClass::Mode_8O1
#define SERIAL_8M1 UARTClass::Mode_8M1
#define SERIAL_8S1 UARTClass::Mode_8S1

#define SERIAL_BUFFER_SIZE 128
#define SERIAL_WRITES_NON_BLOCKING 1

class UARTClass : public HardwareSerial
{
  public:
    enum UARTModes {
      Mode_8N1 = 0,         // No special options
      Mode_7E1 = 0x02,      // CR1 PCE=1, PS=0
      Mode_7O1 = 0x03,      // CR1 PCE=1, PS=1
      Mode_8N1_HALF=0x80,   // 8N1 but supports Half duplex. 
      Mode_8N2 = 0x20,      // CR2 STOP=2
      Mode_7E2 = 0x22,      // CR1 PCE=1, PS=0, CR2 STOP=2
      Mode_7O2 = 0x23,      // CR1 PCE=1, PS=1, CR2 STOP=2
      Mode_8N2_HALF=0xA0,   // 8N2 but supports Half duplex. 

//      Mode_8M1,     // = US_MR_CHRL_8_BIT | US_MR_NBSTOP_1_BIT | UART_MR_PAR_MARK,
//      Mode_8S1,      // = US_MR_CHRL_8_BIT | US_MR_NBSTOP_1_BIT | UART_MR_PAR_SPACE,
    };
    // Setup the Modes to be bit settings: Bits LSB=0, MSB=7
    // 0-1 Parity (CR1)
    // 4-5 Stop bits (CR2 only defined 1=0, 2=2, but .5 and 1.5 are valid as well... )
    // 8 - Half duplex mode (CR3)

    UARTClass(uint8_t uart_num, uint8_t uart_mode, uint8_t *txBuffer, uint16_t tx_buffer_size);
    UARTClass(void);

    void begin(const uint32_t dwBaudRate);
    void begin(const uint32_t dwBaudRate, const UARTModes config);
    void end(void);
    int available(void);
    int availableForWrite(void);
    int peek(void);
    int read(void);
    void flush(void);
    void flushRx( uint32_t timeout_ms );
    size_t write(const uint8_t c);
    size_t write(const uint8_t *buffer, size_t size); 
    using Print::write; // pull in write(str) and write(buf, size) from Print


    void     setDxlMode(bool dxl_mode);
    void TxHandler(void); /* Vassilis Serasidis */
    uint32_t getBaudRate(void);
    uint32_t getRxCnt(void);
    uint32_t getTxCnt(void);

    operator bool() { return true; }; // UART always active


  protected:
    void inline startNextTransmitDMAorIT(void);
    struct tx_no_cache_buffer 
    {
      uint8_t *buffer;
      uint16_t buffer_size;
      volatile uint16_t iHead;
      volatile uint16_t iTail;
    };

    uint8_t  _uart_num;
    uint8_t  _uart_mode;
    uint32_t _uart_baudrate;

    uint8_t r_byte;
    volatile uint16_t    tx_write_size;
    tx_no_cache_buffer tx_buffer;
    uint32_t rx_cnt;
    uint32_t tx_cnt;
};

#endif // _UART_CLASS_
