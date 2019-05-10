/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/usbd_cdc_interface.c
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    29-April-2016
  * @brief   Source file for USBD CDC interface
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright � 2016 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "hw.h"
#include "swtimer.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_RX_DATA_SIZE  512
#define APP_TX_DATA_SIZE  512


const char *JUMP_BOOT_STR = "CM9X";


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_CDC_LineCodingTypeDef LineCoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };

uint8_t CDC_Reset_Status = 0;


uint8_t UserRxBuffer[APP_RX_DATA_SIZE];/* Received Data over USB are stored in this buffer */
uint8_t UserTxBuffer[APP_TX_DATA_SIZE];/* Received Data over UART (CDC interface) are stored in this buffer */
uint8_t UserTxBufferForUSB[APP_TX_DATA_SIZE];/* Received Data over UART (CDC interface) are stored in this buffer */

uint32_t BuffLength;
volatile uint32_t UserTxBufPtrIn = 0;/* Increment this pointer or roll it back to
                               start address when data are received over USART */
volatile uint32_t UserTxBufPtrOut = 0; /* Increment this pointer or roll it back to
                                 start address when data are sent over USB */


volatile uint8_t  rxd_buffer[APP_RX_DATA_SIZE];
volatile uint32_t rxd_length    = 0;
volatile uint32_t rxd_BufPtrIn  = 0;
volatile uint32_t rxd_BufPtrOut = 0;

swtimer_handle_t h_cdc_tx_timer = -1;


static bool is_opened  = false;
static bool is_reopen  = false;
static bool is_tx_full = false;



/* USB handler declaration */
extern USBD_HandleTypeDef  USBD_Device;

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_Itf_Init     (void);
static int8_t CDC_Itf_DeInit   (void);
static int8_t CDC_Itf_Control  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive  (uint8_t* pbuf, uint32_t *Len);

void CDC_Itf_TxISR(void *arg);




USBD_CDC_ItfTypeDef USBD_CDC_fops =
{
  CDC_Itf_Init,
  CDC_Itf_DeInit,
  CDC_Itf_Control,
  CDC_Itf_Receive
};

uint32_t usb_cdc_bitrate = 0;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CDC_Itf_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Init(void)
{

  USBD_CDC_SetTxBuffer(&USBD_Device, UserTxBuffer, 0);
  USBD_CDC_SetRxBuffer(&USBD_Device, UserRxBuffer);

  is_opened       = false;
  UserTxBufPtrIn  = 0;
  UserTxBufPtrOut = 0;

  rxd_length      = 0;
  rxd_BufPtrIn    = 0;
  rxd_BufPtrOut   = 0;

  // 타이머는 최초 실행시 한번만 실행한다.
  if (h_cdc_tx_timer < 0)
  {
    h_cdc_tx_timer = swtimerGetHandle();
    swtimerSet(h_cdc_tx_timer, 3, LOOP_TIME, CDC_Itf_TxISR, NULL );
    swtimerStart(h_cdc_tx_timer);
  }

  return (USBD_OK);
}

/**
  * @brief  CDC_Itf_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_DeInit(void)
{
  return (USBD_OK);
}

/**
  * @brief  CDC_Itf_Control
  *         Manage the CDC class requests
  * @param  Cmd: Command code
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Control (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  USBD_SetupReqTypedef *req = (USBD_SetupReqTypedef *)pbuf;


  switch (cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:
    /* Add your code here */
    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:
    /* Add your code here */
    break;

  case CDC_SET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_GET_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_CLEAR_COMM_FEATURE:
    /* Add your code here */
    break;

  case CDC_SET_LINE_CODING:
    LineCoding.bitrate    = (uint32_t)(pbuf[0] | (pbuf[1] << 8) |\
                            (pbuf[2] << 16) | (pbuf[3] << 24));
    LineCoding.format     = pbuf[4];
    LineCoding.paritytype = pbuf[5];
    LineCoding.datatype   = pbuf[6];

    usb_cdc_bitrate = LineCoding.bitrate;

    if( LineCoding.bitrate == 1200 )
    {
      wdg_setup(10);
      wdg_start();
    }
    break;

  case CDC_GET_LINE_CODING:
    pbuf[0] = (uint8_t)(LineCoding.bitrate);
    pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
    pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
    pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
    pbuf[4] = LineCoding.format;
    pbuf[5] = LineCoding.paritytype;
    pbuf[6] = LineCoding.datatype;
    break;

  case CDC_SET_CONTROL_LINE_STATE:
    /* Add your code here */
    CDC_Reset_Status = 1;

    is_opened = req->wValue&0x01;
    is_reopen = true;
    break;

  case CDC_SEND_BREAK:
     /* Add your code here */
    break;

  default:
    break;
  }

  return (USBD_OK);
}

void CDC_Itf_TxISR(void *arg)
{
  uint32_t buffptr;
  uint32_t buffsize;
  USBD_CDC_HandleTypeDef   *hcdc = USBD_Device.pClassData;


  if(hcdc == NULL)
  {
    return;
  }
  if(hcdc->TxState != 0)
  {
    return;
  }


  if(UserTxBufPtrOut != UserTxBufPtrIn)
  {
    if(UserTxBufPtrOut > UserTxBufPtrIn) /* Rollback */
    {
      buffsize = APP_TX_DATA_SIZE - UserTxBufPtrOut;
    }
    else
    {
      buffsize = UserTxBufPtrIn - UserTxBufPtrOut;
    }

    buffptr = UserTxBufPtrOut;

    memcpy(UserTxBufferForUSB, (uint8_t*)&UserTxBuffer[buffptr], buffsize);
    USBD_CDC_SetTxBuffer(&USBD_Device, UserTxBufferForUSB, buffsize);

    if(USBD_CDC_TransmitPacket(&USBD_Device) == USBD_OK)
    {
      UserTxBufPtrOut += buffsize;
      if (UserTxBufPtrOut == APP_TX_DATA_SIZE)
      {
        UserTxBufPtrOut = 0;
      }
    }
  }
}


/**
  * @brief  CDC_Itf_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  * @param  Buf: Buffer of data to be transmitted
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Receive(uint8_t* Buf, uint32_t *Len)
{

  uint32_t i;


  for( i=0; i<*Len; i++ )
  {
    rxd_buffer[rxd_BufPtrIn] = Buf[i];

    rxd_BufPtrIn++;

    /* To avoid buffer overflow */
    if(rxd_BufPtrIn == APP_RX_DATA_SIZE)
    {
      rxd_BufPtrIn = 0;
    }
  }

  if( CDC_Reset_Status == 1 )
  {
    CDC_Reset_Status = 0;

    if( *Len >= 4 )
    {
      for( i=0; i<3; i++ )
      {
        if( JUMP_BOOT_STR[i] != Buf[i] ) break;
      }

      if( i == 3 )
      {
        wdg_setup(10);
        wdg_start();
      }
    }
  }

  USBD_CDC_ReceivePacket(&USBD_Device);
  return (USBD_OK);
}



int32_t CDC_Itf_Write( uint8_t *p_buf, uint32_t length )
{
  uint32_t i;
  uint32_t ptr_index;


  if( USBD_Device.pClassData == NULL )
  {
    return -1;
  }
  if( is_opened == FALSE && is_reopen == FALSE )
  {
    return -1;
  }
  if( USBD_Device.dev_state != USBD_STATE_CONFIGURED )
  {
    return -1;
  }
  if (length >= CDC_Itf_TxAvailable())
  {
    if (is_tx_full)
    {
      return -1;
    }
    else
    {
      return 0;
    }
  }
  else
  {
    is_tx_full = false;
  }

  __disable_irq();
  ptr_index = UserTxBufPtrIn;


  for (i=0; i<length; i++)
  {

    UserTxBuffer[ptr_index] = p_buf[i];

    ptr_index++;

    /* To avoid buffer overflow */
    if(ptr_index == APP_TX_DATA_SIZE)
    {
      ptr_index = 0;
    }
  }
  UserTxBufPtrIn = ptr_index;
  __enable_irq();

  return length;
}

uint32_t CDC_Itf_TxAvailable( void )
{
  uint32_t length = 0;


  length = (APP_TX_DATA_SIZE + UserTxBufPtrIn - UserTxBufPtrOut) % APP_TX_DATA_SIZE;
  length = APP_TX_DATA_SIZE - length - 1;

  return length;
}

void  CDC_Itf_SetTxFull( void )
{
  is_tx_full = true;
}

uint32_t CDC_Itf_RxAvailable( void )
{
  uint32_t length = 0;


  if( rxd_BufPtrIn != rxd_BufPtrOut )
  {
    length = (APP_RX_DATA_SIZE + rxd_BufPtrIn - rxd_BufPtrOut) % APP_RX_DATA_SIZE;
  }

  return length;
}

uint8_t CDC_Itf_Getch( void )
{

  while(1)
  {
    if( CDC_Itf_RxAvailable() ) break;
  }


  return CDC_Itf_Read();
}


uint8_t CDC_Itf_Read( void )
{
  uint8_t ch = 0;
  uint32_t buffptr;


  buffptr = rxd_BufPtrOut;

  ch = rxd_buffer[buffptr];

  __disable_irq();
  rxd_BufPtrOut += 1;
  if (rxd_BufPtrOut == APP_RX_DATA_SIZE)
  {
    rxd_BufPtrOut = 0;
  }
  __enable_irq();

  return ch;
}

bool CDC_Itf_IsConnected(void)
{


  if( USBD_Device.pClassData == NULL )
  {
    return false;
  }
  if( is_opened == FALSE )
  {
    return false;
  }
  if( USBD_Device.dev_state != USBD_STATE_CONFIGURED )
  {
    return false;
  }

  return true;
}

/*---------------------------------------------------------------------------
     TITLE   : CDC_Itf_Peek
     WORK    :
---------------------------------------------------------------------------*/
int32_t CDC_Itf_Peek( void )
{

  if( rxd_BufPtrIn == rxd_BufPtrOut ) return -1;


  return rxd_buffer[rxd_BufPtrOut];
}

void CDC_Itf_flush_tx ( )
{
  // Try to speed it up as well...
  if (UserTxBufPtrIn != UserTxBufPtrOut) {
    swTimerSetCnt(h_cdc_tx_timer, 1);  // try to force output on the next ms

    // Wait for the a flush to complete 
    while (UserTxBufPtrIn != UserTxBufPtrOut) {
      ;
    }
  }
}

BOOL CDC_Itf_IsTxTransmitted( void )
{
  return (UserTxBufPtrIn == UserTxBufPtrOut) ? TRUE : FALSE;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
