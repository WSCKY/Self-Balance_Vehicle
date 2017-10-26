#ifndef __DEBUG_H
#define __DEBUG_H

#include "stm32f10x_usart.h"

#define DEBUG_USART                    USART1
#define DEBUG_USART_GPIO               GPIOA
#define DEBUG_USART_CLK                RCC_APB2Periph_USART1
#define DEBUG_USART_GPIO_CLK           RCC_APB2Periph_GPIOA
#define DEBUG_USART_RxPin              GPIO_Pin_10
#define DEBUG_USART_TxPin              GPIO_Pin_9
#define DEBUG_USART_Tx_DMA_Channel     DMA1_Channel4
#define DEBUG_USART_Tx_DMA_FLAG        DMA1_FLAG_TC4
#define DEBUG_USART_DR_Base            0x40013804
#define DEBUG_USART_IRQn               USART1_IRQn
#define DEBUG_USART_IRQHandler         USART1_IRQHandler
#define DEBUG_USART_Tx_DMA_IRQn        DMA1_Channel4_IRQn
#define DEBUG_USART_Tx_DMA_IRQHandler  DMA1_Channel4_IRQHandler

void DebugPortInit(void);
void DebugPortSendBytesDMA(uint8_t *p, uint32_t l);

#endif /* __DEBUG_H */
