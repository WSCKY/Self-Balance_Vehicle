#include "MainCtrl.h"

static uint8_t _init_flag = 0;
static uint16_t _ctrl_ticks = 0;

static uint8_t RunEnableFlag = 0;
static uint8_t CmdButtonRleased = 0;
static uint16_t ButtonConfirmTime = 0;
static uint16_t ButtonConfirmTimeCnt = 0;

static MPU_RAW *pMPU;
static EulerAngle *pEulerAngle;
static uint8_t IMU_Stabled = 0;
static GyrRawDef GyrOffset = {0, 0, 0};

static TURN_DIR ExpDirL = STOP, ExpDirR = STOP;
static float ExpSpeedL = 0, ExpSpeedR = 0;

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

	SpeedComputeTask();

	if(!IMU_Stabled)
		IMU_StableCheck();

	if(IMU_Stabled) {
		FusionIMU_6Axis(0.001f);
	}

	if(BUTTON_PRESSED()) {
		if(CmdButtonRleased == 1) {
			if(RunEnableFlag) ButtonConfirmTime = RUN_DISABLE_CONFIRM;
			else ButtonConfirmTime = RUN_ENABLE_CONFIRM;

			if(ButtonConfirmTimeCnt < ButtonConfirmTime)
				ButtonConfirmTimeCnt ++;
			else {
				RunEnableFlag ^= 1;
				CmdButtonRleased = 0;
			}
		}
	} else {
		CmdButtonRleased = 1;
		ButtonConfirmTime = 0;
		ButtonConfirmTimeCnt = 0;
	}

	if(ABS(pEulerAngle->pitch) > 45.0f) {
		RunEnableFlag = 0;
	}

	AttitudeControlLoop(-1.5f, RunEnableFlag);
	SpeedControlLoop(0, RunEnableFlag);

	if(RunEnableFlag == 0) {
		SetRunningDir(STOP, STOP);
		SetRunningSpeed(0, 0);
		LED_OFF();
	} else {
		GetAttitudeControllerOutput(&ExpSpeedL, &ExpSpeedR);
		ExpSpeedL += GetSpeedControllerOutput();
		ExpSpeedR += GetSpeedControllerOutput();
		if(ExpSpeedL >= 0) ExpDirL = FWD; else ExpDirL = REV;
		if(ExpSpeedR >= 0) ExpDirR = FWD; else ExpDirR = REV;
		SetRunningDir(ExpDirL, ExpDirR);
		SetRunningSpeed((uint16_t)ABS(ExpSpeedL), (uint16_t)ABS(ExpSpeedR));
		LED_ON();
	}

	_ctrl_ticks ++;
	if(_ctrl_ticks >= 60000)
		_ctrl_ticks = 0;
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
