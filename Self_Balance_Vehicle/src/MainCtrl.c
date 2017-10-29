#include "MainCtrl.h"

static uint8_t _init_flag = 0;

static MPU_RAW *pMPU;
 EulerAngle *pEulerAngle;
static uint8_t IMU_Stabled = 0;
static GyrRawDef GyrOffset = {0, 0, 0};

#define SpeedReadDiv         (10)
uint8_t SpeedReadDelay = 0;
int16_t MotorSpeed_L = 0, MotorSpeed_R = 0;
float SpeedFilted_L = 0.0f, SpeedFilted_R = 0.0f;

static uint8_t IMU_StableCheck(void);

void MainCtrlLoopInit(void)
{
	pMPU = GetMPU_RawDataPointer();
	pEulerAngle = GetAttitudeAngle();
}

void SystemControlTask(void) /* 1ms */
{
	if(_init_flag == 0) {
		_init_flag = 1;

		MainCtrlLoopInit();
	}
	MPU6500_Read(&GyrOffset, IMU_Stabled);

	/* Read Per (SpeedReadDiv)ms */
	SpeedReadDelay ++;
	if(SpeedReadDelay >= SpeedReadDiv) {
		SpeedReadDelay = 0;
		MotorSpeed_L = ReadEncoderCounter(Encoder_L);
		MotorSpeed_R = ReadEncoderCounter(Encoder_R);
	}

	SpeedFilted_L = SpeedFilted_L * 0.9f + MotorSpeed_L * 0.1f;
	SpeedFilted_R = SpeedFilted_R * 0.9f + MotorSpeed_R * 0.1f;

//	if(!IMU_Stabled)
		IMU_StableCheck();

	if(IMU_Stabled) {
		FusionIMU_6Axis(0.001f);
	}
}
//uint16_t tub = 0;
static uint16_t StableTimeCnt = 0;
static int16_t old_gx = 0, old_gy = 0, old_gz = 0;
/*
 * IMU stabled check task.
 */
static uint8_t IMU_StableCheck(void)
{
	uint8_t ret = 0;
//	tub = (ABS(pMPU->gyrX - old_gx) + ABS(pMPU->gyrY - old_gy) + ABS(pMPU->gyrZ - old_gz));
	if((ABS(pMPU->gyrX - old_gx) + ABS(pMPU->gyrY - old_gy) + ABS(pMPU->gyrZ - old_gz)) < 5) {
		if(StableTimeCnt < 1000)
			StableTimeCnt ++;
		else {
			GyrOffset.gyrX = pMPU->gyrX;
			GyrOffset.gyrY = pMPU->gyrY;
			GyrOffset.gyrZ = pMPU->gyrZ;
			IMU_Stabled = 1; /* already stabled. */
			ret = 1;
		}
	} else {
		StableTimeCnt = 0;
	}
	old_gx = pMPU->gyrX; old_gy = pMPU->gyrY; old_gz = pMPU->gyrZ;
	return ret;
}
