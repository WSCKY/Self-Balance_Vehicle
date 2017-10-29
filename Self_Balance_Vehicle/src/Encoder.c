#include "Encoder.h"

static void EncoderA_Init(void);
static void EncoderB_Init(void);

/*
 * Initialize Encoders(L & R) Driver.
 */
void EncoderDriverInit(void)
{
	EncoderA_Init();
	EncoderB_Init();
}

int16_t ReadEncoderCounter(EncoderID_Def id)
{
	int16_t cnt = 0;
	if(id) {
		cnt = (int16_t)ENCODER_B_TIM->CNT;
		ENCODER_B_TIM->CNT = 0;
	} else {
		cnt = (int16_t)ENCODER_A_TIM->CNT;
		ENCODER_A_TIM->CNT = 0;
	}
	return cnt;
}

/*
 * Configure Encoder A Timer.
 */
static void EncoderA_Init(void)
{
	/* Initialize Structure */
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	/* Enable periph clock */
	RCC_APB1PeriphClockCmd(ENCODER_A_TIM_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(ENCODER_A_GPIO_PORT_CLK, ENABLE);

	/* Configure GPIO PIN as alternate function mode */
	GPIO_InitStructure.GPIO_Pin = ENCODER_A_GPIO_PIN_1 | ENCODER_A_GPIO_PIN_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ENCODER_A_GPIO_PORT, &GPIO_InitStructure);

	/* Configure Timer as Encoder Interface */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENCODER_A_TIM, &TIM_TimeBaseStructure);
	/* Counter counts on both TI1FP1 and TI2FP2 edges && specifies the IC1 Polarity is Rising edge */
	TIM_EncoderInterfaceConfig(ENCODER_A_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  /* Configure Input Capture Parameters */
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 10;
	TIM_ICInit(ENCODER_A_TIM, &TIM_ICInitStructure);

	/* Enable the Encoder Timer Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ENCODER_A_TIM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_ClearFlag(ENCODER_A_TIM, TIM_FLAG_Update);
	/* Enable Update interrupt */
	TIM_ITConfig(ENCODER_A_TIM, TIM_IT_Update, ENABLE);
	TIM_SetCounter(ENCODER_A_TIM, 0); /* Reset Counter Register */
	TIM_Cmd(ENCODER_A_TIM, ENABLE);
}

/*
 * Configure Encoder B Timer.
 */
static void EncoderB_Init(void)
{
	/* Initialize Structure */
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	/* Enable periph clock */
	RCC_APB1PeriphClockCmd(ENCODER_B_TIM_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(ENCODER_B_GPIO_PORT_CLK, ENABLE);

	/* Configure GPIO PIN as alternate function mode */
	GPIO_InitStructure.GPIO_Pin = ENCODER_B_GPIO_PIN_1 | ENCODER_B_GPIO_PIN_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(ENCODER_B_GPIO_PORT, &GPIO_InitStructure);

	/* Configure Timer as Encoder Interface */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(ENCODER_B_TIM, &TIM_TimeBaseStructure);
	/* Counter counts on both TI1FP1 and TI2FP2 edges && specifies the IC1 Polarity is Rising edge */
	TIM_EncoderInterfaceConfig(ENCODER_B_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
  /* Configure Input Capture Parameters */
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 10;
	TIM_ICInit(ENCODER_B_TIM, &TIM_ICInitStructure);

	/* Enable the Encoder Timer Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ENCODER_B_TIM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_ClearFlag(ENCODER_B_TIM, TIM_FLAG_Update);
	/* Enable Update interrupt */
	TIM_ITConfig(ENCODER_B_TIM, TIM_IT_Update, ENABLE);
	TIM_SetCounter(ENCODER_B_TIM, 0); /* Reset Counter Register */
	TIM_Cmd(ENCODER_B_TIM, ENABLE);
}

/*
 * this function handles Encoder A Timer interrupt.
 */
void ENCODER_A_TIM_Handler(void)
{
	if(ENCODER_A_TIM->SR & 0X0001) {
		/* __nop(); */
	}
	ENCODER_A_TIM->SR &= ~(1 << 0);
}

/*
 * this function handles Encoder B Timer interrupt.
 */
void ENCODER_B_TIM_Handler(void)
{
	if(ENCODER_B_TIM->SR & 0X0001) {
		/* __nop(); */
	}
	ENCODER_B_TIM->SR &= ~(1 << 0);
}
