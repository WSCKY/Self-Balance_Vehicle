#include "SpeedController.h"

static uint8_t _init_flag = 0;
static uint16_t ControllerTicks = 0;

uint8_t SpeedReadDelay = 0;
int16_t MotorSpeed_L = 0, MotorSpeed_R = 0;
float SpeedFilted_L = 0.0f, SpeedFilted_R = 0.0f;

void SpeedComputeTask(void)
{
	/* Read Per (SpeedReadDiv)ms */
	SpeedReadDelay ++;
	if(SpeedReadDelay >= SpeedReadDiv) {
		SpeedReadDelay = 0;
		MotorSpeed_L = ReadEncoderCounter(Encoder_L);
		MotorSpeed_R = ReadEncoderCounter(Encoder_R);
	}
	SpeedFilted_L = SpeedFilted_L * 0.9f + MotorSpeed_L * 0.1f;
	SpeedFilted_R = SpeedFilted_R * 0.9f + MotorSpeed_R * 0.1f;
}

static PID SpeedPID = {0};

static void ControllerInit(void)
{
	SpeedPID.dt = 0.01f;
	SpeedPID.kp = 2.8f;
	SpeedPID.ki = 0.1f;
	SpeedPID.I_max = 200.0f;
	SpeedPID.I_sum = 0.0f;
}

void SpeedControlLoop(float ExpVel, uint8_t ControllerEnable)
{
	if(_init_flag == 0) {
		_init_flag = 1;

		ControllerInit();
	}

	if(ControllerTicks % 10 == 0) {
		pid_loop(&SpeedPID, ExpVel, (SpeedFilted_L + SpeedFilted_R));
	}

	if(ControllerEnable == 0) {
		SpeedPID.I_sum = 0;
	}

	ControllerTicks ++;
	if(ControllerTicks >= 60000)
		ControllerTicks = 0;
}

float GetSpeedControllerOutput(void)
{
	return (-SpeedPID.Output);
}
