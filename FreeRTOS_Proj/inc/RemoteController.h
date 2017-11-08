#ifndef __REMOTECONTROLLER_H
#define __REMOTECONTROLLER_H

#include "stm32f10x_usart.h"

#define RC_USART                    USART3
#define RC_USART_GPIO               GPIOB
#define RC_USART_GPIO_CLK           RCC_APB2Periph_GPIOB
#define RC_USART_CLK                RCC_APB1Periph_USART3
#define RC_USART_RxPin              GPIO_Pin_11
#define RC_USART_TxPin              GPIO_Pin_10
#define RC_USART_IRQn               USART3_IRQn
#define RC_USART_IRQHandler         USART3_IRQHandler

#define RC_MIDDLE_VALUE             1024

__packed typedef struct
{
	uint8_t startByte;
	__packed union
	{
		uint16_t data8[22];
		__packed struct
		{
			uint16_t data1 :11;
			uint16_t data2 :11;
			uint16_t data3 :11;
			uint16_t data4 :11;
			uint16_t data5 :11;
			uint16_t data6 :11;
			uint16_t data7 :11;
			uint16_t data8 :11;
			uint16_t data9 :11;
			uint16_t data10 :11;
			uint16_t data11 :11;
			uint16_t data12 :11;
			uint16_t data13 :11;
			uint16_t data14 :11;
			uint16_t data15 :11;
			uint16_t data16 :11;
		} data11;
	} rf_data;
	uint8_t flags;
	uint8_t endByte;
} RC_DATA_t;

typedef struct {
	uint16_t Channel[16];
} RC_CHANNLE_t;

void RemoteControlInit(void);
uint8_t GetRCUpdateFlag(void);
RC_CHANNLE_t *GetRC_ChannelData(void);
void RC_ParseData(void);

#endif /* __REMOTECONTROLLER_H */
