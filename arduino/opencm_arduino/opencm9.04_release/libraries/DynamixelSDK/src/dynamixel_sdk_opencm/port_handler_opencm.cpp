/*******************************************************************************
* Copyright (c) 2016, ROBOTIS CO., LTD.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this
*   list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
*
* * Neither the name of ROBOTIS nor the names of its
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/* Author: Ryu Woon Jung (Leon) */

#include <Arduino.h>


#include "../../include/dynamixel_sdk_opencm/port_handler_opencm.h"

//#define DYNAMIXEL_SERIAL  Serial3


#define LATENCY_TIMER     4  // msec (USB latency timer)

using namespace dynamixel;

PortHandlerOpenCM::PortHandlerOpenCM(const char *port_name)
  : baudrate_(DEFAULT_BAUDRATE_),
    packet_start_time_(0.0),
    packet_timeout_(0.0),
    tx_time_per_byte(0.0)
{
  is_using_ = false;
  setPortName(port_name);

  if (port_name[0] == '1')
  {
    socket_fd_ = 0;
    p_dxl_serial = &Serial1;
  }
  else if (port_name[0] == '2')
  {
    socket_fd_ = 1;
    p_dxl_serial = &Serial2;
  }
  else if (port_name[0] == '3')
  {
    socket_fd_ = 2;
    p_dxl_serial = &Serial3;
  }
  else
  {
    socket_fd_ = 0;
    p_dxl_serial = &Serial1;
  }

  drv_dxl_begin(socket_fd_);

  setTxDisable();
}

bool PortHandlerOpenCM::openPort()
{
  return setBaudRate(baudrate_);
}

void PortHandlerOpenCM::closePort()
{
  setPowerOff();
}

void PortHandlerOpenCM::clearPort()
{
  p_dxl_serial->flush();
}

void PortHandlerOpenCM::setPortName(const char *port_name)
{
  strcpy(port_name_, port_name);
}

char *PortHandlerOpenCM::getPortName()
{
  return port_name_;
}

bool PortHandlerOpenCM::setBaudRate(const int baudrate)
{
  baudrate_ = checkBaudrateAvailable(baudrate);


  if (baudrate_ == -1)
    return false;

  setupPort(baudrate_);


  return true;
}

int PortHandlerOpenCM::getBaudRate()
{
  return baudrate_;
}

int PortHandlerOpenCM::getBytesAvailable()
{
  int bytes_available;

  bytes_available = p_dxl_serial->available();

  return bytes_available;
}

int PortHandlerOpenCM::readPort(uint8_t *packet, int length)
{
  int rx_length;

  rx_length = p_dxl_serial->available();
  if (rx_length > length)
    rx_length = length;

  for (int i = 0; i < rx_length; i++)
  {
    packet[i] = p_dxl_serial->read();
  }

  return rx_length;
}

int PortHandlerOpenCM::writePort(uint8_t *packet, int length)
{
  int length_written;

  setTxEnable();

  length_written = p_dxl_serial->write(packet, length);

  setTxDisable();

  return length_written;
}

void PortHandlerOpenCM::setPacketTimeout(uint16_t packet_length)
{
  packet_start_time_  = getCurrentTime();
  packet_timeout_     = (tx_time_per_byte * (double)packet_length) + (LATENCY_TIMER * 2.0) + 2.0;
}

void PortHandlerOpenCM::setPacketTimeout(double msec)
{
  packet_start_time_  = getCurrentTime();
  packet_timeout_     = msec;
}

bool PortHandlerOpenCM::isPacketTimeout()
{
  if (getTimeSinceStart() > packet_timeout_)
  {
    packet_timeout_ = 0;
    return true;
  }

  return false;
}

double PortHandlerOpenCM::getCurrentTime()
{
	return (double)millis();
}

double PortHandlerOpenCM::getTimeSinceStart()
{
  double elapsed_time;

  elapsed_time = getCurrentTime() - packet_start_time_;
  if (elapsed_time < 0.0)
    packet_start_time_ = getCurrentTime();

  return elapsed_time;
}

bool PortHandlerOpenCM::setupPort(int baudrate)
{
  p_dxl_serial->setDxlMode(true);
  p_dxl_serial->begin(baudrate);
  delay(100);

  tx_time_per_byte = (1000.0 / (double)baudrate) * 10.0;
  return true;
}

int PortHandlerOpenCM::checkBaudrateAvailable(int baudrate)
{
  switch(baudrate)
  {
    case 9600:
      return 9600;
    case 57600:
      return 57600;
    case 115200:
      return 115200;
    case 1000000:
      return 1000000;
    case 2000000:
      return 2000000;
    case 3000000:
      return 3000000;
    case 4000000:
      return 4000000;
    case 4500000:
      return 4500000;
    default:
      return -1;
  }
}

void PortHandlerOpenCM::setPowerOn()
{

}

void PortHandlerOpenCM::setPowerOff()
{

}

void PortHandlerOpenCM::setTxEnable()
{
  drv_dxl_tx_enable(socket_fd_, TRUE);
}

void PortHandlerOpenCM::setTxDisable()
{
  drv_dxl_tx_enable(socket_fd_, FALSE);
}
