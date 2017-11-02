#include "AttitudeController.h"

static uint8_t _init_flag = 0;
static uint16_t ControllerTicks = 0;

static PID AnglePID = {0};
static PID GyrosPID = {0};

static GyrDataDef *pGyr;
static float GyroY = 0.0f;
static EulerAngle *pEulerAngle;

static void ControllerInit(void)
{
	pGyr = GetGyrDataPointer();
	pEulerAngle = GetAttitudeAngle();

	AnglePID.dt = (float)ANGLE_CTRL_RATE_DIV / (float)SYSTEM_LOOP_RATE;
	AnglePID.kp = 20.0f;
	AnglePID.ki = 0.0f;
	AnglePID.I_max = 0.0f;
	AnglePID.I_sum = 0.0f;

	GyrosPID.dt = (float)GYROS_CTRL_RATE_DIV / (float)SYSTEM_LOOP_RATE;
	GyrosPID.kp = 1.0f;
	GyrosPID.ki = 0.0f;
	GyrosPID.I_max = 100.0f;
	GyrosPID.I_sum = 0.0f;
}

void AttitudeControlLoop(float ExpAngle, uint8_t ControllerEnable)
{
	if(_init_flag == 0) {
		_init_flag = 1;

		ControllerInit();
	}

	if(ControllerTicks % ANGLE_CTRL_RATE_DIV == 0) {
		pid_loop(&AnglePID, ExpAngle, pEulerAngle->pitch);
	}

	GyroY = GyroY * 0.7f + pGyr->gyrY * 0.3f;
	if(ControllerTicks % GYROS_CTRL_RATE_DIV == 0) {
		pid_loop(&GyrosPID, AnglePID.Output, GyroY);
	}

	if(ControllerEnable == 0) {
		GyrosPID.I_sum = 0;
		AnglePID.I_sum = 0;
	}

	ControllerTicks ++;
	if(ControllerTicks >= 60000)
		ControllerTicks = 0;
}

void GetAttitudeControllerOutput(float *Left, float *Right)
{
	*Left = -GyrosPID.Output;
	*Right = -GyrosPID.Output;
}
