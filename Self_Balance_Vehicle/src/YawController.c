#include "YawController.h"

static uint8_t _init_flag = 0;
static uint16_t ControllerTicks = 0;

static GyrDataDef *pGyr;
static PID YawCtrlPID = {0};

static void ControllerInit(void)
{
	pGyr = GetGyrDataPointer();

	YawCtrlPID.dt = (float)YAW_CTRL_RATE_DIV / (float)SYSTEM_LOOP_RATE;
	YawCtrlPID.kp = 2.0f;
	YawCtrlPID.ki = 2.0f;
	YawCtrlPID.I_max = 100.0f;
	YawCtrlPID.I_sum = 0.0f;
}

void YawControlLoop(float ExpYawRate, uint8_t ControllerEnable)
{
	if(_init_flag == 0) {
		_init_flag = 1;

		ControllerInit();
	}

	if(ControllerTicks % YAW_CTRL_RATE_DIV == 0) {
		pid_loop(&YawCtrlPID, ExpYawRate, pGyr->gyrZ);
	}

	if(ControllerEnable == 0) {
		YawCtrlPID.I_sum = 0.0f;
	}

	ControllerTicks ++;
	if(ControllerTicks >= 60000)
		ControllerTicks = 0;
}

float GetYawControllerOutput(void)
{
	return YawCtrlPID.Output;
}
