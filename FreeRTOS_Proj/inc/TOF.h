#ifndef __TOF_H
#define __TOF_H

#include "stm32f10x_usart.h"

#include "CRC16.h"

#define TOF_USART                    USART2
#define TOF_USART_GPIO               GPIOA
#define TOF_USART_GPIO_CLK           RCC_APB2Periph_GPIOA
#define TOF_USART_CLK                RCC_APB1Periph_USART2
#define TOF_USART_RxPin              GPIO_Pin_3
#define TOF_USART_TxPin              GPIO_Pin_2
#define TOF_USART_IRQn               USART2_IRQn
#define TOF_USART_IRQHandler         USART2_IRQHandler

__packed typedef struct
{
	uint8_t Header1;
	uint8_t Header2;
	uint8_t HighByte;
	uint8_t LowByte;
	uint8_t HighCRC;
	uint8_t LowCRC;
} TOF_ORIGIN_DATA;

void TOFDriverInit(void);
uint8_t GetNewTOFData(float *d);

#endif /* __TOF_H */
