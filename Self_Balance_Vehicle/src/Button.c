#include "Button.h"

/*
 * Configure Button GPIO periph.
 */
void ButtonInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* BUTTON_GPIO Periph clock enable */
	RCC_APB2PeriphClockCmd(BUTTON_GPIO_PORT_CLK, ENABLE);

	/* Configure BUTTON_PIN as input floating */
	GPIO_InitStructure.GPIO_Pin = BUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
	GPIO_Init(BUTTON_GPIO_PORT, &GPIO_InitStructure);
}
