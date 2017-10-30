#include "RemoteController.h"

static RC_DATA_t RC_Data = {0};
static uint8_t RCDataUpdate = 0;

static RC_CHANNLE_t RC_ChannelData = {0};

static void usart_config(void);
static void _rc_data_decode(uint8_t data);

void RemoteControlInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RC_USART_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

	/* Configure RC_USART Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = RC_USART_RxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(RC_USART_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = RC_USART_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(RC_USART_GPIO, &GPIO_InitStructure);

	usart_config();
}

static void usart_config(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable RC_USART Clock */
  RCC_APB1PeriphClockCmd(RC_USART_CLK, ENABLE);

	/* RC USART configuration -------------------------------------------*/
  /* RC USART configured as follow:
        - BaudRate = 100000
        - Word Length = 8 Bits
        - Two Stop Bit
        - Even parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */

  USART_InitStructure.USART_BaudRate = 100000;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_2;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  /* Configure RC_USART */
  USART_Init(RC_USART, &USART_InitStructure);

	/* Enable the RC_USART Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RC_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable the RC_USART Receive Interrupt */
  USART_ITConfig(RC_USART, USART_IT_RXNE, ENABLE);

  /* Enable RC_USART */
  USART_Cmd(RC_USART, ENABLE);
}

uint8_t GetRCUpdateFlag(void)
{
	if(RCDataUpdate) {
		RCDataUpdate = 0;
		return 1;
	}
	return 0;
}

RC_CHANNLE_t *GetRC_ChannelData(void)
{
	return &RC_ChannelData;
}

void RC_ParseData(void)
{
	RC_ChannelData.Channel[0]  = ((RC_Data.rf_data.data8[0] | RC_Data.rf_data.data8[1] << 8) & 0x07FF);
	RC_ChannelData.Channel[1]  = ((RC_Data.rf_data.data8[1] >> 3 | RC_Data.rf_data.data8[2] << 5) & 0x07FF);
	RC_ChannelData.Channel[2]  = ((RC_Data.rf_data.data8[2] >> 6 | RC_Data.rf_data.data8[3] << 2 | RC_Data.rf_data.data8[4] << 10) & 0x07FF);
	RC_ChannelData.Channel[3]  = ((RC_Data.rf_data.data8[4] >> 1 | RC_Data.rf_data.data8[5] << 7) & 0x07FF);
	RC_ChannelData.Channel[4]  = ((RC_Data.rf_data.data8[5] >> 4 | RC_Data.rf_data.data8[6] << 4) & 0x07FF);
	RC_ChannelData.Channel[5]  = ((RC_Data.rf_data.data8[6] >> 7 | RC_Data.rf_data.data8[7] << 1 | RC_Data.rf_data.data8[8] << 9) & 0x07FF);
	RC_ChannelData.Channel[6]  = ((RC_Data.rf_data.data8[8] >> 2 | RC_Data.rf_data.data8[9] << 6) & 0x07FF);
	RC_ChannelData.Channel[7]  = ((RC_Data.rf_data.data8[9] >> 5 | RC_Data.rf_data.data8[10]<<3) & 0x07FF); // & the other 8 + 2 channels if you need them
	RC_ChannelData.Channel[8]  = ((RC_Data.rf_data.data8[11] | RC_Data.rf_data.data8[12] << 8) & 0x07FF);
	RC_ChannelData.Channel[9]  = ((RC_Data.rf_data.data8[12] >> 3 | RC_Data.rf_data.data8[13] << 5) & 0x07FF);
	RC_ChannelData.Channel[10] = ((RC_Data.rf_data.data8[13] >> 6 | RC_Data.rf_data.data8[14] << 2 | RC_Data.rf_data.data8[15] << 10) & 0x07FF);
	RC_ChannelData.Channel[11] = ((RC_Data.rf_data.data8[15] >> 1 | RC_Data.rf_data.data8[16] << 7) & 0x07FF);
	RC_ChannelData.Channel[12] = ((RC_Data.rf_data.data8[16] >> 4 | RC_Data.rf_data.data8[17] << 4) & 0x07FF);
	RC_ChannelData.Channel[13] = ((RC_Data.rf_data.data8[17] >> 7 | RC_Data.rf_data.data8[18] << 1 | RC_Data.rf_data.data8[19] << 9) & 0x07FF);
	RC_ChannelData.Channel[14] = ((RC_Data.rf_data.data8[19] >> 2 | RC_Data.rf_data.data8[20] << 6) & 0x07FF);
	RC_ChannelData.Channel[15] = ((RC_Data.rf_data.data8[20] >> 5 | RC_Data.rf_data.data8[21] << 3) & 0x07FF);
}

static uint8_t _data_step = 0;
static uint8_t _rx_data_cnt = 0;
static RC_DATA_t _rc_data = {0};
static void _rc_data_decode(uint8_t data)
{
	switch(_data_step) {
		case 0: {
			if(data == 0x0F) {
				_rc_data.startByte = data;
				_rx_data_cnt = 0;
				_data_step = 1;
			}
		} break;
		case 1: {
			_rc_data.rf_data.data8[_rx_data_cnt] = data;
			_rx_data_cnt ++;
			if(_rx_data_cnt >= 22)
				_data_step = 2;
		} break;
		case 2: {
			_rc_data.flags = data;
			_data_step = 3;
		} break;
		case 3: {
			_data_step = 0;
			_rc_data.endByte = data;
			if(data == 0x04 || data == 0x14 || data == 0x24 || data == 0x34) {
				if((_rc_data.flags & 0x0C) != 0x04 && (_rc_data.flags & 0x0C) != 0x0C) { //useless data.
					RC_Data = _rc_data;
					RCDataUpdate = 1;
				}
			}
		} break;
		default: _data_step = 0; break;
	}
}

void RC_USART_IRQHandler(void)
{
	if(SET == USART_GetITStatus(RC_USART, USART_IT_RXNE)) {
		USART_ClearFlag(RC_USART, USART_FLAG_RXNE);
		USART_GetFlagStatus(RC_USART, USART_FLAG_ORE);
		_rc_data_decode(USART_ReceiveData(RC_USART));
	}
}
