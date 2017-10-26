#include "TB6612FNG.h"

static void timer_config(void);

void MotorDriverInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIO Periph clock enable */
	RCC_APB2PeriphClockCmd(TB_AIN_GPIO_PORT_CLK | TB_BIN_GPIO_PORT_CLK | \
	                       TB_PWMA_GPIO_PORT_CLK | TB_PWMB_GPIO_PORT_CLK | RCC_APB2Periph_AFIO, ENABLE);

	/* Configure CTRL_PIN in output pushpull mode */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = TB_AIN_1_GPIO_PIN | TB_AIN_2_GPIO_PIN;
	GPIO_Init(TB_AIN_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = TB_BIN_1_GPIO_PIN | TB_BIN_2_GPIO_PIN;
	GPIO_Init(TB_BIN_GPIO_PORT, &GPIO_InitStructure);
	/* Stop Mode */
	GPIO_ResetBits(TB_AIN_GPIO_PORT, TB_AIN_1_GPIO_PIN | TB_AIN_2_GPIO_PIN);
	GPIO_ResetBits(TB_BIN_GPIO_PORT, TB_BIN_1_GPIO_PIN | TB_BIN_2_GPIO_PIN);
	/* Configure PWM_PIN as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = TB_PWM_A_GPIO_PIN;
	GPIO_Init(TB_PWMA_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = TB_PWM_B_GPIO_PIN;
	GPIO_Init(TB_PWMB_GPIO_PORT, &GPIO_InitStructure);

	timer_config();
}

static void timer_config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	uint16_t PrescalerValue = 0;

	/* TB_TIMER clock enable */
  RCC_APB2PeriphClockCmd(TB_TIMER_CLK, ENABLE);

	/* -----------------------------------------------------------------------
    TB_TIMER Configuration: generate 2 PWM signals with 2 different duty cycles:
    The TB_TIMER_CLK frequency is set to SystemCoreClock (Hz), to get TB_TIMER counter
    clock at 8 MHz the Prescaler is computed as following:
     - Prescaler = (TB_TIMER_CLK / TB_TIMER counter clock) - 1
    SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 8 MHz for Low-Density Value line and
    Medium-Density Value line devices

    The TB_TIMER is running at 10 KHz: TB_TIMER Frequency = TB_TIMER counter clock/(ARR + 1)
                                                  = 8 MHz / 800 = 10 KHz
    TB_TIMER Channel1 duty cycle = (TB_TIMER_CCR1/ TB_TIMER_ARR) * 100
    TB_TIMER Channel4 duty cycle = (TB_TIMER_CCR4/ TB_TIMER_ARR) * 100
  ----------------------------------------------------------------------- */
  /* Compute the prescaler value */
  PrescalerValue = (uint16_t) (SystemCoreClock / 8000000) - 1;
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 799;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TB_TIMER, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_Pulse = 0;

  TIM_OC1Init(TB_TIMER, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TB_TIMER, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel4 */
  TIM_OCInitStructure.TIM_Pulse = 0;

  TIM_OC4Init(TB_TIMER, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TB_TIMER, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TB_TIMER, ENABLE);

  /* TB_TIMER enable counter */
  TIM_Cmd(TB_TIMER, ENABLE);

	/* TB_TIMER Main Output Enable */
  TIM_CtrlPWMOutputs(TB_TIMER, ENABLE);
}

void SetRunningDir(TURN_DIR dir_L, TURN_DIR dir_R)
{
	if(dir_L == FWD) MOTOR_B_FWD();
	else if(dir_L == REV) MOTOR_B_REV();
	else MOTOR_B_STOP();

	if(dir_R == FWD) MOTOR_A_FWD();
	else if(dir_R == REV) MOTOR_A_REV();
	else MOTOR_A_STOP();
}

void SetRunningSpeed(uint16_t spd_L, uint16_t spd_R)
{
	TB_TIMER->CCR1 = spd_L;
	TB_TIMER->CCR4 = spd_R;
}
