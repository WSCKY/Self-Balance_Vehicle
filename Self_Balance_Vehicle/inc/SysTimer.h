#ifndef __SYSTIMER_H
#define __SYSTIMER_H

#include "stm32f10x_tim.h"

#define SYS_TIMER            TIM2
#define SYS_TIMER_CLK        RCC_APB1Periph_TIM2

#define SYS_TIMER_IRQn       TIM2_IRQn
#define SYS_TIMER_Handler    TIM2_IRQHandler

void SysTimerInit(void);
void SystemControlTask(void);

#endif /* __SYSTIMER_H */
