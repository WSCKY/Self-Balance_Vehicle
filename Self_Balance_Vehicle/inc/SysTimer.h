#ifndef __SYSTIMER_H
#define __SYSTIMER_H

#include "SysConfig.h"
#include "stm32f10x_tim.h"

#define SYS_TIMER            TIM3
#define SYS_TIMER_CLK        RCC_APB1Periph_TIM3

#define SYS_TIMER_IRQn       TIM3_IRQn
#define SYS_TIMER_Handler    TIM3_IRQHandler

void SysTimerInit(void);
void SystemControlTask(void);

#endif /* __SYSTIMER_H */
