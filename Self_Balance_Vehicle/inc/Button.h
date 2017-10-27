#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f10x_gpio.h"

#define BUTTON_GPIO_PORT               GPIOA
#define BUTTON_GPIO_PORT_CLK           RCC_APB2Periph_GPIOA

#define BUTTON_GPIO_PIN                GPIO_Pin_15

#define BUTTON_STATE()                 (BUTTON_GPIO_PORT->IDR & BUTTON_GPIO_PIN)
#define BUTTON_PRESSED()               (BUTTON_STATE() == RESET)

#endif /* __BUTTON_H */
