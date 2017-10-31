#include "SpeedController.h"

static uint8_t _init_flag = 0;
static uint16_t ControllerTicks = 0;

static uint8_t SpeedReadDelay = 0;
static int16_t MotorSpeed_L = 0, MotorSpeed_R = 0;
static float SpeedFilted_L = 0.0f, SpeedFilted_R = 0.0f;

void SpeedComputeTask(void)
{
	/* Read Per (SPEED_READ_RATE_DIV * 5)ms */
	SpeedReadDelay ++;
	if(SpeedReadDelay >= SPEED_READ_RATE_DIV) {
		SpeedReadDelay = 0;
		MotorSpeed_L = ReadEncoderCounter(Encoder_L);
		MotorSpeed_R = ReadEncoderCounter(Encoder_R);
	}
	SpeedFilted_L = SpeedFilted_L * 0.7f + MotorSpeed_L * 0.3f;
	SpeedFilted_R = SpeedFilted_R * 0.7f + MotorSpeed_R * 0.3f;
}

static PID SpeedPID = {0};

static void ControllerInit(void)
{
	SpeedPID.dt = (float)SPEED_CTRL_RATE_DIV / (float)SYSTEM_LOOP_RATE;
	SpeedPID.kp = 6.0f;
	SpeedPID.ki = 1.0f;
	SpeedPID.I_max = 300.0f;
	SpeedPID.I_sum = 0.0f;
}

void SpeedControlLoop(float ExpVel, uint8_t ControllerEnable)
{
	if(_init_flag == 0) {
		_init_flag = 1;

		ControllerInit();
	}

	if(ControllerTicks % SPEED_CTRL_RATE_DIV == 0) {
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
