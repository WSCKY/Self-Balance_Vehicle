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
static uint16_t DelayTicksCnt = 0;

static uint16_t LEDFlashDelay = 0;
static uint16_t DebugDataSendDelay = 0;
/* Private function prototypes ----------------------------------------------*/
/* Private functions --------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	DelayInit();
	MPU6050_Init();
	TOFDriverInit();
	DebugPortInit();
	MotorDriverInit();
	EncoderDriverInit();
	RemoteControlInit();
	VoltageMeasureInit();

	EstimatorInit();
	SysTimerInit();

	LEDFlashDelay = 1000 / LED_FLASH_RATE_WAIT_STABLE;
	DebugDataSendDelay = 1000 / DEBUG_DATA_FRAME_RATE;
  /* Infinite loop */
  while (1)
  {
		if(DelayTicksCnt % DebugDataSendDelay == 0) {
			SendDataToMonitor();
		}

		if(!IMU_GotOffset()) {
			LEDFlashDelay = 1000 / LED_FLASH_RATE_WAIT_STABLE;
		} else if(GetSignalLostFlag()) {
			LEDFlashDelay = 1000 / LED_FLASH_RATE_SIGNAL_LOST;
		} else {
			LEDFlashDelay = 0;
		}

		if(LEDFlashDelay != 0) {
			if(DelayTicksCnt % LEDFlashDelay == 0)
				LED_TOG();
		} else {
			if(GetVehicleRunState()) {
				LED_ON();
			} else {
				LED_OFF();
			}
		}

		Delay(1);
		DelayTicksCnt ++;
		if(DelayTicksCnt >= 60000)
			DelayTicksCnt = 0;
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
