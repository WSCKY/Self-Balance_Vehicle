#ifndef __LED_H
#define __LED_H

#include "stm32f10x_gpio.h"

#define LED_GPIO_PORT        GPIOA
#define LED_GPIO_PORT_CLK    RCC_APB2Periph_GPIOA

#define LED_GPIO_PIN         GPIO_Pin_12

#define LED_ON()             LED_GPIO_PORT->BRR = LED_GPIO_PIN;
#define LED_OFF()            LED_GPIO_PORT->BSRR = LED_GPIO_PIN;
#define LED_TOG()            LED_GPIO_PORT->ODR ^= LED_GPIO_PIN;

void LED_Init(void);

#endif /* __LED_H */
