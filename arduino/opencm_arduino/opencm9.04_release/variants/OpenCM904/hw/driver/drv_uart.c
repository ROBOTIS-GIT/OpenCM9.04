/*
 *  drv_uart.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram, PBHP
 */
/*

  USART1
    - RX : DMA1, Channel 5
    - TX : DMA1, Channel 4

  USART2
    - RX : DMA1, Channel 6
    - TX : DMA1, Channel 7

  USART3
    - RX : DMA1, Channel 3
    - TX : DMA1, Channel 2
*/
#include "drv_uart.h"
#include "variant.h"


//-- internal definition
//
#define DRV_UART_RX_BUF_LENGTH      256


//-- internal variable
//
static uint32_t drv_uart_rx_buf_head[DRV_UART_NUM_MAX];
static uint32_t drv_uart_rx_buf_tail[DRV_UART_NUM_MAX];
static uint8_t  drv_uart_rx_buf[DRV_UART_NUM_MAX][DRV_UART_RX_BUF_LENGTH];


static uint8_t  drv_uart_num = 0;

static BOOL is_init[DRV_UART_NUM_MAX];
static BOOL is_uart_mode[DRV_UART_NUM_MAX];
static BOOL is_dxl_port[DRV_UART_NUM_MAX];


UART_HandleTypeDef huart[DRV_UART_NUM_MAX];
DMA_HandleTypeDef  hdma_rx[DRV_UART_NUM_MAX];
DMA_HandleTypeDef  hdma_tx[DRV_UART_NUM_MAX];
USART_TypeDef     *huart_inst[DRV_UART_NUM_MAX] = { USART1, USART2, USART3 };


//-- internal functions definition
//
void drv_uart_err_handler(uint8_t uart_num);




int drv_uart_init()
{
  uint8_t i;


  for(i=0; i<DRV_UART_NUM_MAX; i++)
  {
    is_init[i]      = FALSE;
    is_uart_mode[i] = DRV_UART_IRQ_MODE;
    is_dxl_port[i]  = FALSE;

    drv_uart_rx_buf_head[i] = 0;
    drv_uart_rx_buf_tail[i] = 0;
  }

  return 0;
}

void drv_uart_begin(uint8_t uart_num, uint8_t uart_mode, uint32_t baudrate)
{
  if(uart_num < DRV_UART_NUM_MAX)
  {
    huart[uart_num].Instance          = huart_inst[uart_num];
    huart[uart_num].Init.BaudRate     = baudrate;
    huart[uart_num].Init.WordLength   = UART_WORDLENGTH_8B;
    huart[uart_num].Init.StopBits     = UART_STOPBITS_1;
    huart[uart_num].Init.Parity       = UART_PARITY_NONE;
    huart[uart_num].Init.Mode         = UART_MODE_TX_RX;
    huart[uart_num].Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart[uart_num].Init.OverSampling = UART_OVERSAMPLING_16;

    drv_uart_num = uart_num;
    is_uart_mode[uart_num] = uart_mode;
    
    HAL_UART_DeInit(&huart[uart_num]);
    HAL_UART_Init(&huart[uart_num]);

    is_init[uart_num] = TRUE;

    drv_uart_start_rx(uart_num);

    drv_uart_rx_buf_head[uart_num] = DRV_UART_RX_BUF_LENGTH - hdma_rx[uart_num].Instance->CNDTR;
    drv_uart_rx_buf_tail[uart_num] = drv_uart_rx_buf_head[uart_num];
  }
}

void drv_uart_set_dxl_mode(uint8_t uart_num, BOOL dxl_mode)
{
  if(uart_num < DRV_UART_NUM_MAX)
  {
    is_dxl_port[uart_num] = dxl_mode;
  }
}

uint32_t drv_uart_write(uint8_t uart_num, const uint8_t wr_data)
{
  HAL_UART_Transmit(&huart[uart_num], (uint8_t *)&wr_data, 1, 10);
  return 1;
}

uint32_t drv_uart_write_dma_it(uint8_t uart_num, const uint8_t *wr_data, uint16_t Size)
{
  // call the DMA or IT function depending on if configured for DMA or not.
  if (is_uart_mode[uart_num]) 
  {
    return (uint32_t)HAL_UART_Transmit_DMA(&huart[uart_num], (uint8_t *)wr_data, Size);  
  }
  return (uint32_t)HAL_UART_Transmit_IT(&huart[uart_num], (uint8_t *)wr_data, Size);  
}


void drv_uart_flush(uint8_t uart_num)
{
  UNUSED(uart_num);
}

void drv_uart_rx_flush(uint8_t uart_num, uint32_t timeout_ms)
{
  uint32_t pre_time_ms = millis();
  while((drv_uart_read(uart_num) != -1) || (millis() - pre_time_ms < timeout_ms))
  {
  }
}

void drv_uart_start_rx(uint8_t uart_num)
{
  if(is_uart_mode[uart_num] == DRV_UART_IRQ_MODE)
  {
    HAL_UART_Receive_IT(&huart[uart_num], (uint8_t *)drv_uart_rx_buf[uart_num], 1);
  }
  else
  {
    HAL_UART_Receive_DMA(&huart[uart_num], (uint8_t *)drv_uart_rx_buf[uart_num], DRV_UART_RX_BUF_LENGTH );
  }
}

uint32_t drv_uart_read_buf(uint8_t uart_num, uint8_t *p_buf, uint32_t length)
{
  uint32_t i;
  uint32_t ret = 0;

  if(is_uart_mode[uart_num] == DRV_UART_IRQ_MODE)
  {
    for( i=0; i<length; i++)
    {
      p_buf[i] = drv_uart_rx_buf[uart_num][i];
    }
    ret = length;
  }
  else
  {

  }

  return ret;
}

uint8_t drv_uart_get_mode(uint8_t uart_num)
{
  return is_uart_mode[uart_num];
}

uint32_t drv_uart_available(uint8_t uart_num)
{
  uint32_t length = 0;

  drv_uart_rx_buf_head[uart_num] = DRV_UART_RX_BUF_LENGTH - hdma_rx[uart_num].Instance->CNDTR;

  length = (   DRV_UART_RX_BUF_LENGTH
             + drv_uart_rx_buf_head[uart_num]
             - drv_uart_rx_buf_tail[uart_num] ) % DRV_UART_RX_BUF_LENGTH;

  return length;
}

uint32_t drv_uart_tx_available(uint8_t uart_num)
{
  (void)(uart_num);
  uint32_t length;

  length = 1;

  return length;
}

int drv_uart_peek(uint8_t uart_num)
{
  int index;

  index = drv_uart_rx_buf_tail[uart_num];

  // Update the head to current state of DMA
  drv_uart_rx_buf_head[uart_num] = DRV_UART_RX_BUF_LENGTH - hdma_rx[uart_num].Instance->CNDTR;

  if (drv_uart_rx_buf_head[uart_num] == index)
  {
    return -1;
  }    

  return drv_uart_rx_buf[uart_num][index];
}

int drv_uart_read(uint8_t uart_num)
{
    int ret   = -1;
    int index = drv_uart_rx_buf_tail[uart_num];

    // Update the head to current state of DMA
    drv_uart_rx_buf_head[uart_num] = DRV_UART_RX_BUF_LENGTH - hdma_rx[uart_num].Instance->CNDTR;

    if (drv_uart_rx_buf_head[uart_num] != index)
    {
      ret = drv_uart_rx_buf[uart_num][index];
      drv_uart_rx_buf_tail[uart_num] = (drv_uart_rx_buf_tail[uart_num] + 1) % DRV_UART_RX_BUF_LENGTH;
    }

    return ret;
}

void drv_uart_err_handler(uint8_t uart_num)
{
  drv_uart_flush(uart_num);
  drv_uart_start_rx(uart_num);
}




void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart[DRV_UART_NUM_1]);
}


void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart[DRV_UART_NUM_2]);
}

void USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart[DRV_UART_NUM_3]);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  if( UartHandle->Instance == huart_inst[DRV_UART_NUM_1] ) Tx1_Handler();
  if( UartHandle->Instance == huart_inst[DRV_UART_NUM_2] ) Tx2_Handler();
  if( UartHandle->Instance == huart_inst[DRV_UART_NUM_3] ) Tx3_Handler();

}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  if( UartHandle->Instance == huart_inst[DRV_UART_NUM_1] ) drv_uart_err_handler(DRV_UART_NUM_1);
  if( UartHandle->Instance == huart_inst[DRV_UART_NUM_2] ) drv_uart_err_handler(DRV_UART_NUM_2);
  if( UartHandle->Instance == huart_inst[DRV_UART_NUM_3] ) drv_uart_err_handler(DRV_UART_NUM_3);
}


// UART1 DMA IRQ
void DMA1_Channel5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(huart[DRV_UART_NUM_1].hdmarx);
}

// UART2 DMA IRQ
void DMA1_Channel6_IRQHandler(void)
{
  HAL_DMA_IRQHandler(huart[DRV_UART_NUM_2].hdmarx);
}

// UART3 DMA IRQ
void DMA1_Channel3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(huart[DRV_UART_NUM_3].hdmarx);
}

// TX handlers
// UART1 DMA IRQ
void DMA1_Channel4_IRQHandler(void)
{
  HAL_DMA_IRQHandler(huart[DRV_UART_NUM_1].hdmatx);
}

// UART2 DMA IRQ
void DMA1_Channel7_IRQHandler(void)
{
  HAL_DMA_IRQHandler(huart[DRV_UART_NUM_2].hdmatx);
}

// UART3 DMA IRQ
void DMA1_Channel2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(huart[DRV_UART_NUM_3].hdmatx);
}


void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct;


  if(huart->Instance == USART1)  // // UART_NUM_1 / UART_NUM_4;
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();


    if (is_dxl_port[drv_uart_num] == FALSE)
    {
      /* UART TX GPIO pin configuration  */
      GPIO_InitStruct.Pin       = GPIO_PIN_9;
      GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull      = GPIO_PULLUP;
      GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


      /* UART RX GPIO pin configuration  */
      GPIO_InitStruct.Pin       = GPIO_PIN_10;
      GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else
    {
      __HAL_AFIO_REMAP_USART1_ENABLE();

      /* UART TX GPIO pin configuration  */
      GPIO_InitStruct.Pin       = GPIO_PIN_6;
      GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull      = GPIO_PULLUP;
      GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


      /* UART RX GPIO pin configuration  */
      GPIO_InitStruct.Pin       = GPIO_PIN_7;
      GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }


    // DMA Setup
    /* Configure the DMA handler for reception process */
    hdma_rx[DRV_UART_NUM_1].Instance                 = DMA1_Channel5;
    hdma_rx[DRV_UART_NUM_1].Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx[DRV_UART_NUM_1].Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx[DRV_UART_NUM_1].Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx[DRV_UART_NUM_1].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx[DRV_UART_NUM_1].Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx[DRV_UART_NUM_1].Init.Mode                = DMA_CIRCULAR;
    hdma_rx[DRV_UART_NUM_1].Init.Priority            = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&hdma_rx[DRV_UART_NUM_1]);

    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_rx[DRV_UART_NUM_1]);

    /* NVIC configuration for DMA transfer complete interrupt (USART1_RX) */
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    // Now see if we configured for TX to be DMA...
    if(is_uart_mode[DRV_UART_NUM_1] == DRV_UART_DMA_MODE) 
    {
      hdma_tx[DRV_UART_NUM_1].Instance                 = DMA1_Channel4;
      hdma_tx[DRV_UART_NUM_1].Init.Direction           = DMA_MEMORY_TO_PERIPH;
      hdma_tx[DRV_UART_NUM_1].Init.PeriphInc           = DMA_PINC_DISABLE;
      hdma_tx[DRV_UART_NUM_1].Init.MemInc              = DMA_MINC_ENABLE;
      hdma_tx[DRV_UART_NUM_1].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_tx[DRV_UART_NUM_1].Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
      hdma_tx[DRV_UART_NUM_1].Init.Mode                = DMA_NORMAL;
      hdma_tx[DRV_UART_NUM_1].Init.Priority            = DMA_PRIORITY_MEDIUM;

      HAL_DMA_Init(&hdma_tx[DRV_UART_NUM_1]);

      /* Associate the initialized DMA handle to the the UART handle */
      __HAL_LINKDMA(huart, hdmatx, hdma_tx[DRV_UART_NUM_1]);

      HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    }

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ  (USART1_IRQn);
  }
  else if(huart->Instance==USART2)  // // UART_NUM_2
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();


    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_2;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    // DMA Setup
    /* Configure the DMA handler for reception process */
    hdma_rx[DRV_UART_NUM_2].Instance                 = DMA1_Channel6;
    hdma_rx[DRV_UART_NUM_2].Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx[DRV_UART_NUM_2].Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx[DRV_UART_NUM_2].Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx[DRV_UART_NUM_2].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx[DRV_UART_NUM_2].Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx[DRV_UART_NUM_2].Init.Mode                = DMA_CIRCULAR;
    hdma_rx[DRV_UART_NUM_2].Init.Priority            = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&hdma_rx[DRV_UART_NUM_2]);

    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_rx[DRV_UART_NUM_2]);

    /* NVIC configuration for DMA transfer complete interrupt (USART1_RX) */
    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

    // Now see if we configured for TX to be DMA...
    if(is_uart_mode[DRV_UART_NUM_2] == DRV_UART_DMA_MODE) 
    {

      hdma_tx[DRV_UART_NUM_2].Instance                 = DMA1_Channel7;
      hdma_tx[DRV_UART_NUM_2].Init.Direction           = DMA_MEMORY_TO_PERIPH;
      hdma_tx[DRV_UART_NUM_2].Init.PeriphInc           = DMA_PINC_DISABLE;
      hdma_tx[DRV_UART_NUM_2].Init.MemInc              = DMA_MINC_ENABLE;
      hdma_tx[DRV_UART_NUM_2].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_tx[DRV_UART_NUM_2].Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
      hdma_tx[DRV_UART_NUM_2].Init.Mode                = DMA_NORMAL;
      hdma_tx[DRV_UART_NUM_2].Init.Priority            = DMA_PRIORITY_MEDIUM;

      HAL_DMA_Init(&hdma_tx[DRV_UART_NUM_2]);

      /* Associate the initialized DMA handle to the the UART handle */
      __HAL_LINKDMA(huart, hdmatx, hdma_tx[DRV_UART_NUM_2]);

      HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
    }

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ  (USART2_IRQn);
  }
  else if(huart->Instance==USART3)  // // UART_NUM_3
  {
    /* Peripheral clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_USART3_CLK_ENABLE();

    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_11;
    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    // DMA Setup
    /* Configure the DMA handler for reception process */
    hdma_rx[DRV_UART_NUM_3].Instance                 = DMA1_Channel3;
    hdma_rx[DRV_UART_NUM_3].Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma_rx[DRV_UART_NUM_3].Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_rx[DRV_UART_NUM_3].Init.MemInc              = DMA_MINC_ENABLE;
    hdma_rx[DRV_UART_NUM_3].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx[DRV_UART_NUM_3].Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    hdma_rx[DRV_UART_NUM_3].Init.Mode                = DMA_CIRCULAR;
    hdma_rx[DRV_UART_NUM_3].Init.Priority            = DMA_PRIORITY_HIGH;

    HAL_DMA_Init(&hdma_rx[DRV_UART_NUM_3]);

    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, hdma_rx[DRV_UART_NUM_3]);

    /* NVIC configuration for DMA transfer complete interrupt (USART1_RX) */
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    // Now see if we configured for TX to be DMA...
    if(is_uart_mode[DRV_UART_NUM_3] == DRV_UART_DMA_MODE) 
    {
      hdma_tx[DRV_UART_NUM_3].Instance                 = DMA1_Channel2;
      hdma_tx[DRV_UART_NUM_3].Init.Direction           = DMA_MEMORY_TO_PERIPH;
      hdma_tx[DRV_UART_NUM_3].Init.PeriphInc           = DMA_PINC_DISABLE;
      hdma_tx[DRV_UART_NUM_3].Init.MemInc              = DMA_MINC_ENABLE;
      hdma_tx[DRV_UART_NUM_3].Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      hdma_tx[DRV_UART_NUM_3].Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
      hdma_tx[DRV_UART_NUM_3].Init.Mode                = DMA_NORMAL;
      hdma_tx[DRV_UART_NUM_3].Init.Priority            = DMA_PRIORITY_MEDIUM;

      HAL_DMA_Init(&hdma_tx[DRV_UART_NUM_3]);

      /* Associate the initialized DMA handle to the the UART handle */
      __HAL_LINKDMA(huart, hdmatx, hdma_tx[DRV_UART_NUM_3]);

      HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    }

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ  (USART3_IRQn);
  }
}


void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance == USART1)
  {
    __USART1_FORCE_RESET();
    __USART1_RELEASE_RESET();

    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();


    if (is_dxl_port[drv_uart_num] == FALSE)
    {
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
      HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
    }
    else
    {
      HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
      HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
    }

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART1_IRQn);
    HAL_NVIC_DisableIRQ(DMA1_Channel5_IRQn);
  }
  else if(huart->Instance == USART2)
  {
    __USART2_FORCE_RESET();
    __USART2_RELEASE_RESET();

    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();


    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    HAL_NVIC_DisableIRQ(DMA1_Channel6_IRQn);
  }
  else if(huart->Instance==USART3)
  {
    __USART3_FORCE_RESET();
    __USART3_RELEASE_RESET();

    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();


    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART3_IRQn);
    HAL_NVIC_DisableIRQ(DMA1_Channel3_IRQn);
  }
}
