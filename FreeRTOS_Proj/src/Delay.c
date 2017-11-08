#include "Delay.h"

/* counts 1ms timeTicks */
volatile uint32_t msTicks;

void DelayInit(void)
{
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}

/*
 * Delay specific Ticks.
 */
void Delay(uint16_t Ticks)
{
	msTicks = 0;
	while(msTicks < Ticks);
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
		msTicks ++;
}
