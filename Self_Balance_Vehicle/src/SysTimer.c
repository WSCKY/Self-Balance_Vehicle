#include "SysTimer.h"

/*
 * Initialize System Timer.
 */
void SysTimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* SYS_TIMER clock enable */
  RCC_APB1PeriphClockCmd(SYS_TIMER_CLK, ENABLE);

	/* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = (8000000 / SYSTEM_LOOP_RATE) - 1; /* 200Hz */
  TIM_TimeBaseStructure.TIM_Prescaler = 8;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(SYS_TIMER, &TIM_TimeBaseStructure);

	/* Enable the SYS_TIMER Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = SYS_TIMER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(SYS_TIMER, TIM_IT_Update, ENABLE);

	/* SYS_TIMER enable counter */
  TIM_Cmd(SYS_TIMER, ENABLE);
}

/*
 * this function handles SYS_TIMER interrupt.
 */
void SYS_TIMER_Handler(void)
{
	if(TIM_GetFlagStatus(SYS_TIMER, TIM_FLAG_Update)) {
		SystemControlTask();
		TIM_ClearFlag(SYS_TIMER, TIM_FLAG_Update);
	}
}
