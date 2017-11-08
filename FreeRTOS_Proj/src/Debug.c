#include "Debug.h"

static uint8_t tx_comp_flag = 1;
static DMA_InitTypeDef DMA_InitStructure;

static void dma_config(void);
static void usart_config(void);

void DebugPortInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(DEBUG_USART_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

	/* Configure DEBUG_USART Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(DEBUG_USART_GPIO, &GPIO_InitStructure);

  /* Configure DEBUG_USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(DEBUG_USART_GPIO, &GPIO_InitStructure);

	usart_config();
	dma_config();
}

static void usart_config(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable DEBUG_USART Clock */
  RCC_APB2PeriphClockCmd(DEBUG_USART_CLK, ENABLE);

	/* Debug USART configuration -------------------------------------------*/
  /* Debug USART configured as follow:
        - BaudRate = 115200
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* Configure DEBUG_USART */
  USART_Init(DEBUG_USART, &USART_InitStructure);

	/* Enable the DEBUG_USART Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable DEBUG_USART DMA TX request */
  USART_DMACmd(DEBUG_USART, USART_DMAReq_Tx, ENABLE);

  /* Enable the DEBUG_USART Receive Interrupt */
  USART_ITConfig(DEBUG_USART, USART_IT_RXNE, ENABLE);

  /* Enable DEBUG_USART */
  USART_Cmd(DEBUG_USART, ENABLE);
}

static void dma_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* DEBUG_USART_Tx_DMA_Channel (triggered by DEBUG_USART Tx event) Config */
  DMA_DeInit(DEBUG_USART_Tx_DMA_Channel);
  DMA_InitStructure.DMA_PeripheralBaseAddr = DEBUG_USART_DR_Base;
  DMA_InitStructure.DMA_MemoryBaseAddr = 0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DEBUG_USART_Tx_DMA_Channel, &DMA_InitStructure);

	/* Enable the DEBUG_USART_Tx_DMA_IRQn Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_Tx_DMA_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	DMA_ITConfig(DEBUG_USART_Tx_DMA_Channel, DMA_IT_TC, ENABLE);

//	/* Enable DEBUG_USART DMA TX Channel */
//  DMA_Cmd(DEBUG_USART_Tx_DMA_Channel, ENABLE);
}

/*
 * Send any bytes through debug port use dma.
 */
void DebugPortSendBytesDMA(uint8_t *p, uint32_t l)
{
	if(tx_comp_flag == 1) {
		tx_comp_flag = 0;

		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)p;
		DMA_InitStructure.DMA_BufferSize = l;
		DMA_Init(DEBUG_USART_Tx_DMA_Channel, &DMA_InitStructure);
		/* Enable DEBUG_USART DMA TX Channel */
		DMA_Cmd(DEBUG_USART_Tx_DMA_Channel, ENABLE);
	}
}

/*
 * this function handles debug port interrupt.
 */
void DEBUG_USART_IRQHandler(void)
{
	if(SET == USART_GetITStatus(DEBUG_USART, USART_IT_RXNE)) {
		USART_ReceiveData(DEBUG_USART);
		USART_ClearFlag(DEBUG_USART, USART_FLAG_RXNE);
	}
}

/*
 * this function handles UART DMA Interrupt.
 */
void DEBUG_USART_Tx_DMA_IRQHandler(void)
{
	if(SET == DMA_GetFlagStatus(DMA1_FLAG_TC4)) {
		tx_comp_flag = 1;
		/* Disable DEBUG_USART DMA TX Channel */
		DMA_Cmd(DEBUG_USART_Tx_DMA_Channel, DISABLE);
		DMA_ClearFlag(DMA1_FLAG_TC4);
	}
}
