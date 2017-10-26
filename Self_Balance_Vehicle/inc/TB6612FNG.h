#ifndef __TB6612FNG_H
#define __TB6612FNG_H

#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"

#define TB_AIN_GPIO_PORT               GPIOB
#define TB_AIN_GPIO_PORT_CLK           RCC_APB2Periph_GPIOB
#define TB_BIN_GPIO_PORT               GPIOB
#define TB_BIN_GPIO_PORT_CLK           RCC_APB2Periph_GPIOB

#define TB_AIN_1_GPIO_PIN              GPIO_Pin_12
#define TB_AIN_2_GPIO_PIN              GPIO_Pin_13
#define TB_BIN_1_GPIO_PIN              GPIO_Pin_14
#define TB_BIN_2_GPIO_PIN              GPIO_Pin_15

#define TB_PWMA_GPIO_PORT              GPIOA
#define TB_PWMA_GPIO_PORT_CLK          RCC_APB2Periph_GPIOA
#define TB_PWMB_GPIO_PORT              GPIOA
#define TB_PWMB_GPIO_PORT_CLK          RCC_APB2Periph_GPIOA

#define TB_PWM_A_GPIO_PIN              GPIO_Pin_11
#define TB_PWM_B_GPIO_PIN              GPIO_Pin_8

#define TB_TIMER                       TIM1
#define TB_TIMER_CLK                   RCC_APB2Periph_TIM1

typedef enum {
	STOP = 0,
	FWD = 1,
	REV = 2,
} TURN_DIR;

#define MOTOR_A_STOP()                 do {TB_AIN_GPIO_PORT->BRR = TB_AIN_1_GPIO_PIN; TB_AIN_GPIO_PORT->BRR = TB_AIN_2_GPIO_PIN;} while(0)
#define MOTOR_A_FWD()                  do {TB_AIN_GPIO_PORT->BSRR = TB_AIN_1_GPIO_PIN; TB_AIN_GPIO_PORT->BRR = TB_AIN_2_GPIO_PIN;} while(0)
#define MOTOR_A_REV()                  do {TB_AIN_GPIO_PORT->BRR = TB_AIN_1_GPIO_PIN; TB_AIN_GPIO_PORT->BSRR = TB_AIN_2_GPIO_PIN;} while(0)

#define MOTOR_B_STOP()                 do {TB_BIN_GPIO_PORT->BRR = TB_BIN_1_GPIO_PIN; TB_BIN_GPIO_PORT->BRR = TB_BIN_2_GPIO_PIN;} while(0)
#define MOTOR_B_FWD()                  do {TB_BIN_GPIO_PORT->BRR = TB_BIN_1_GPIO_PIN; TB_BIN_GPIO_PORT->BSRR = TB_BIN_2_GPIO_PIN;} while(0)
#define MOTOR_B_REV()                  do {TB_BIN_GPIO_PORT->BSRR = TB_BIN_1_GPIO_PIN; TB_BIN_GPIO_PORT->BRR = TB_BIN_2_GPIO_PIN;} while(0)

void MotorDriverInit(void);
void SetRunningDir(TURN_DIR dir_L, TURN_DIR dir_R);
void SetRunningSpeed(uint16_t spd_L, uint16_t spd_R);

#endif /* __TB6612FNG_H */
