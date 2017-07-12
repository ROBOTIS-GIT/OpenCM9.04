/*
 *  drv_spi.c
 *
 *  Created on: 2016. 7. 13.
 *      Author: Baram, PBHP
 */

#include "drv_spi.h"
#include "variant.h"


#define SPI_MAX_CH              2
#define SPI_TX_DMA_MAX_LENGTH   0xEFFF



typedef struct
{
  bool use;
  bool init;
  bool tx_done;
  uint8_t *p_tx_buf;
  uint8_t *p_tx_buf_next;
  uint32_t tx_length_next;
} spi_dma_t;


SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;



volatile spi_dma_t spi_dma[SPI_MAX_CH];






int drv_spi_init()
{
  uint8_t i;


  hspi1.Instance                = SPI1;
  hspi1.Init.Mode               = SPI_MODE_MASTER;
  hspi1.Init.Direction          = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize           = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity        = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase           = SPI_PHASE_1EDGE;
  hspi1.Init.NSS                = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit           = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode             = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial      = 10;
  //HAL_SPI_Init(&hspi1);

  hspi2.Instance                = SPI2;
  hspi2.Init.Mode               = SPI_MODE_MASTER;
  hspi2.Init.Direction          = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize           = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity        = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase           = SPI_PHASE_1EDGE;
  hspi2.Init.NSS                = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit           = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode             = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial      = 10;
  //HAL_SPI_Init(&hspi2);

  return 0;
}


void drv_spi_enable_dma(SPI_HandleTypeDef* hspi)
{
#if 0
  if(hspi->Instance==SPI1)
  {
  }
  if(hspi->Instance==SPI2)
  {
    spi_dma[1].use = true;
  }
#endif
}


uint8_t drv_spi_is_dma_tx_done(SPI_HandleTypeDef* hspi)
{
#if 0
  if(hspi->Instance==SPI1)
  {
    if(spi_dma[0].use != true) return true;

    return spi_dma[0].tx_done;
  }
  if(hspi->Instance==SPI2)
  {
    if(spi_dma[1].use != true) return true;

    return spi_dma[1].tx_done;
  }
#endif
  return true;
}


void drv_spi_start_dma_tx(SPI_HandleTypeDef* hspi, uint8_t *p_buf, uint32_t length)
{
#if 0
  if(hspi->Instance==SPI1)
  {
  }
  if(hspi->Instance==SPI2)
  {
    if(spi_dma[1].use != true) return;

    if(length > SPI_TX_DMA_MAX_LENGTH)
    {
      spi_dma[1].tx_done       = false;
      spi_dma[1].tx_length_next= length - SPI_TX_DMA_MAX_LENGTH;
      spi_dma[1].p_tx_buf      =  p_buf;
      spi_dma[1].p_tx_buf_next = &p_buf[SPI_TX_DMA_MAX_LENGTH];

      HAL_SPI_Transmit_DMA(hspi, spi_dma[1].p_tx_buf, SPI_TX_DMA_MAX_LENGTH);
    }
    else
    {
      spi_dma[1].tx_done       = false;
      spi_dma[1].tx_length_next= 0;
      spi_dma[1].p_tx_buf      = p_buf;
      spi_dma[1].p_tx_buf_next = NULL;

      HAL_SPI_Transmit_DMA(hspi, spi_dma[1].p_tx_buf, length);
    }
  }
#endif
}


void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{

  GPIO_InitTypeDef GPIO_InitStruct;


  if(hspi->Instance==SPI1)
  {
    __HAL_RCC_SPI1_CLK_ENABLE();

    GPIO_InitStruct.Pin       = GPIO_PIN_5;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
  if(hspi->Instance==SPI2)
  {
    __HAL_RCC_SPI2_CLK_ENABLE();

    GPIO_InitStruct.Pin       = GPIO_PIN_13;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_14;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI1)
  {
    __HAL_RCC_SPI1_FORCE_RESET();
    __HAL_RCC_SPI1_RELEASE_RESET();
    __HAL_RCC_SPI1_CLK_DISABLE();


    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(SPI1_IRQn);
  }

  if(hspi->Instance==SPI2)
  {
    __HAL_RCC_SPI2_FORCE_RESET();
    __HAL_RCC_SPI2_RELEASE_RESET();
    __HAL_RCC_SPI2_CLK_DISABLE();


    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_14);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_15);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(SPI2_IRQn);
  }
}
