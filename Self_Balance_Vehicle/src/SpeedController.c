#include "SpeedController.h"

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
