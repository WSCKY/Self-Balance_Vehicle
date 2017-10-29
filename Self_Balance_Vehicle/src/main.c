/**
  *****************************************************************************
  * @file    Project/Self_Balance_Vehicle/src/main.c
  * @author  kyChu
  * @version V0.0.1
  * @date    25-October-2017
  * @brief   Main program body.
  *****************************************************************************
  */

/* Includes -----------------------------------------------------------------*/
#include "main.h"

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
TURN_DIR dirL = STOP, dirR = STOP;
uint16_t speedL = 0, speedR = 0;
//uint8_t BeepStart = 0;
//TURN_DIR dir = STOP;
//uint16_t sound = 0;
//uint16_t freq = 500;
/* Private function prototypes ----------------------------------------------*/
/* Private functions --------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	LED_Init();
	DelayInit();
	MPU6050_Init();
	DebugPortInit();
	MotorDriverInit();
	EncoderDriverInit();

	EstimatorInit();
	SysTimerInit();

  /* Infinite loop */
  while (1)
  {
		SendDataToMonitor();
		Delay(5);
		if(BUTTON_PRESSED()) {
			LED_ON();
		} else {
			LED_OFF();
		}

//		LED_TOG();
//		Delay(200);
		SetRunningDir(dirL, dirR);
		SetRunningSpeed(speedL, speedR);

//		if(BeepStart) {
//			SetRunningDir(FWD, FWD);
//			SetRunningSpeed(sound, sound);
//			Delay(freq);
//			SetRunningDir(REV, REV);
//			SetRunningSpeed(sound, sound);
//			Delay(freq);
//		}
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************************** END OF FILE ********************************/
