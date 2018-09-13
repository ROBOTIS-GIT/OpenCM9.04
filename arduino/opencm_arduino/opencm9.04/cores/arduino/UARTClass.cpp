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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "UARTClass.h"
#include "wiring_digital.h"
#include "wiring_constants.h"
// Constructors ////////////////////////////////////////////////////////////////
UARTClass::UARTClass(void){

}

UARTClass::UARTClass(uint8_t uart_num, uint8_t uart_mode, RingBuffer_Typedef* tx_buffer)
{
  _uart_num  = uart_num;
  _uart_mode = uart_mode;
  _uart_tx_buffer = tx_buffer;
  _uart_baudrate = 0;
  rx_cnt = 0;
  tx_cnt = 0;
}

void UARTClass::begin(const uint32_t dwBaudRate)
{
  begin(dwBaudRate, Mode_8N1);
}

void UARTClass::begin(const uint32_t dwBaudRate, const UARTModes config)
{
  _uart_baudrate = dwBaudRate;

  drv_uart_begin(_uart_num, _uart_mode, _uart_tx_buffer, dwBaudRate);
}

void UARTClass::end( void )
{
  drv_uart_flush(_uart_num);
}

int UARTClass::available( void )
{
  return drv_uart_available(_uart_num);
}

int UARTClass::availableForWrite(void)
{
  return drv_uart_tx_available(_uart_num);
}

int UARTClass::peek( void )
{
  return drv_uart_peek(_uart_num);
}

int UARTClass::read( void )
{
  rx_cnt++;
  return drv_uart_read(_uart_num);
}

void UARTClass::flush( void )
{
  drv_uart_flush(_uart_num);
}

void UARTClass::flushRx( uint32_t timeout_ms )
{
  drv_uart_rx_flush(_uart_num, timeout_ms);
}

size_t UARTClass::write( const uint8_t uc_data )
{
  tx_cnt++;
  return drv_uart_write(_uart_num, uc_data);
}

size_t UARTClass::write( const uint8_t *buffer, size_t size )
{
  tx_cnt += size;
  return drv_uart_write_buf(_uart_num, buffer, size);

}

uint32_t UARTClass::getBaudRate( void )
{
  return _uart_baudrate;
}

uint32_t UARTClass::getRxCnt(void)
{
  return rx_cnt;
}

uint32_t UARTClass::getTxCnt(void)
{
  return tx_cnt;
}

void UARTClass::transmitterEnable(uint8_t pin) 
{
  if (pin < 0x100)  pinMode(pin, OUTPUT); // make sure enabled as output pin. 
  drv_uart_set_transmit_enable_pin(_uart_num, pin);
}

void UARTClass::setDxlMode(bool dxl_mode)
{
  drv_uart_set_dxl_mode(_uart_num, dxl_mode);
}
