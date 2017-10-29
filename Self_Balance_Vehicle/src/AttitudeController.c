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

	AnglePID.dt = 0.01f;
	AnglePID.kp = 20.0f;
	AnglePID.ki = 0.4f;
	AnglePID.I_max = 80.0f;
	AnglePID.I_sum = 0.0f;

	GyrosPID.dt = 0.005f;
	GyrosPID.kp = 1.9f;
	GyrosPID.ki = 0.0f;
	GyrosPID.I_max = 0.0f;
	GyrosPID.I_sum = 0.0f;
}

void AttitudeControlLoop(float ExpAngle, uint8_t ControllerEnable)
{
	if(_init_flag == 0) {
		_init_flag = 1;

		ControllerInit();
	}

	if(ControllerTicks % 10 == 0) {
		pid_loop(&AnglePID, ExpAngle, pEulerAngle->pitch);
	}

	GyroY = GyroY * 0.9f + pGyr->gyrY * 0.1f;
	if(ControllerTicks % 5 == 0) {
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
