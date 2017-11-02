#include "TOF.h"

static uint8_t _tof_data_update = 0;
static TOF_ORIGIN_DATA TOF_DATA = {0};

static void usart_config(void);
static void _rc_data_decode(uint8_t data);

void TOFDriverInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(TOF_USART_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

	/* Configure TOF_USART Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = TOF_USART_RxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(TOF_USART_GPIO, &GPIO_InitStructure);

	usart_config();
}

static void usart_config(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable TOF_USART Clock */
  RCC_APB1PeriphClockCmd(TOF_USART_CLK, ENABLE);

	/* RC USART configuration -------------------------------------------*/
  /* RC USART configured as follow:
        - BaudRate = 115200
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_2;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx;
  /* Configure TOF_USART */
  USART_Init(TOF_USART, &USART_InitStructure);

	/* Enable the TOF_USART Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TOF_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the TOF_USART Receive Interrupt */
  USART_ITConfig(TOF_USART, USART_IT_RXNE, ENABLE);

  /* Enable TOF_USART */
  USART_Cmd(TOF_USART, ENABLE);
}

uint8_t GetNewTOFData(float *d)
{
	if(_tof_data_update) {
		*d = (((uint16_t)TOF_DATA.HighByte << 8) | TOF_DATA.LowByte) / 10.0f;
		return 1;
	}
	return 0;
}

static uint8_t _data_step = 0;
static TOF_ORIGIN_DATA _tof_recv = {0};
static void _rc_data_decode(uint8_t data)
{
	switch(_data_step) {
		case 0: {
			if(data == 0xA5) {
				_tof_recv.Header1 = data;
				_data_step ++;
			}
		} break;
		case 1: {
			if(data == 0x5A) {
				_tof_recv.Header2 = data;
				_data_step ++;
			} else {
				_data_step = 0;
			}
		} break;
		case 2: {
			_tof_recv.HighByte = data;
			_data_step ++;
		} break;
		case 3: {
			_tof_recv.LowByte = data;
			_data_step ++;
		} break;
		case 4: {
			_tof_recv.LowCRC = data;
			_data_step ++;
		} break;
		case 5: {
			_tof_recv.HighCRC = data;
			if((((uint16_t)_tof_recv.HighCRC << 8) | _tof_recv.LowCRC) == crc16((uint8_t *)&_tof_recv, 4)) {
				TOF_DATA = _tof_recv;
				_tof_data_update = 1;
			}
			_data_step = 0;
		} break;
		default: _data_step = 0; break;
	}
}

void TOF_USART_IRQHandler(void)
{
	if(SET == USART_GetITStatus(TOF_USART, USART_IT_RXNE)) {
		_rc_data_decode(USART_ReceiveData(TOF_USART));
		USART_ClearFlag(TOF_USART, USART_FLAG_RXNE);
		USART_ClearITPendingBit(TOF_USART, USART_IT_RXNE);
	}
}
