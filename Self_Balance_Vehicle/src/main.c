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
//uint8_t SendString[] = "Hello kyChu!\n";
//TURN_DIR dirL = STOP, dirR = STOP;
//uint16_t speedL = 0, speedR = 0;
AccDataDef *pAcc;
GyrDataDef *pGyr;
float temp;
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

	SysTimerInit();

	pAcc = GetAccDataPointer();
	pGyr = GetGyrDataPointer();

  /* Infinite loop */
  while (1)
  {
		LED_TOG();
		Delay(200);
//		SetRunningDir(dirL, dirR);
//		SetRunningSpeed(speedL, speedR);
//		MPU6500_Read();
//		temp = GetMPU6050Temperature();
//		DebugPortSendBytesDMA(SendString, 13);
  }
}

void SystemControlTask(void)
{
	MPU6500_Read();
	temp = GetMPU6050Temperature();
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
