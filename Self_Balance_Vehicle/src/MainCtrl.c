#include "MainCtrl.h"

static uint8_t _init_flag = 0;
static uint16_t _ctrl_ticks = 0;
static float MainCtrlLoopDt = 0.0f;

static uint8_t RunEnableFlag = 0;
static uint8_t CmdButtonRleased = 0;
static uint16_t ButtonConfirmTime = 0;
static uint16_t ButtonConfirmTimeCnt = 0;

static MPU_RAW *pMPU;
static EulerAngle *pEulerAngle;
static uint8_t IMU_Stabled = 0;
static GyrRawDef GyrOffset = {0, 0, 0};

static RC_CHANNLE_t *pRC;
static uint8_t SignalLostFlag = 1;
static uint32_t SignalLostCnt = 0;

static TURN_DIR ExpDirL = STOP, ExpDirR = STOP;
static float ExpSpeedL = 0, ExpSpeedR = 0;

static uint8_t IMU_StableCheck(void);

void MainCtrlLoopInit(void)
{
	MainCtrlLoopDt = 1.0f / SYSTEM_LOOP_RATE;
	pMPU = GetMPU_RawDataPointer();
	pEulerAngle = GetAttitudeAngle();

	pRC = GetRC_ChannelData();
}
float exp_vel = 0, exp_yaw = 0;
void SystemControlTask(void) /* SYSTEM_LOOP_RATE Hz */
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
		FusionIMU_6Axis(MainCtrlLoopDt);
	}

	if(GetRCUpdateFlag()) {
		SignalLostCnt = 0;
		SignalLostFlag = 0;
		RC_ParseData();
	} else {
		if(SignalLostCnt < 200)
			SignalLostCnt ++;
		else {
			SignalLostFlag = 1;
		}
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

	if(ABS(pEulerAngle->pitch) > 50.0f || \
		IMU_Stabled == 0) {
		RunEnableFlag = 0;
	}

	AttitudeControlLoop(-1.8f, RunEnableFlag);
	if(SignalLostFlag) {
		exp_vel = 0; exp_yaw = 0;
	} else {
		exp_vel = ((1024 - pRC->Channel[1]) * 2 / 35.0f);
		exp_yaw = ((1024 - pRC->Channel[0]) * 8 / 35.0f);
	}
	SpeedControlLoop(exp_vel, RunEnableFlag);
	YawControlLoop(exp_yaw, RunEnableFlag);

	if(RunEnableFlag == 0) {
		SetRunningDir(STOP, STOP);
		SetRunningSpeed(0, 0);
	} else {
		GetAttitudeControllerOutput(&ExpSpeedL, &ExpSpeedR); // apply attitude control.
		ExpSpeedL += GetSpeedControllerOutput(); // apply speed control.
		ExpSpeedR += GetSpeedControllerOutput();
		ExpSpeedL -= GetYawControllerOutput(); // apply yaw control.
		ExpSpeedR += GetYawControllerOutput();
		if(ExpSpeedL >= 0) ExpDirL = FWD; else ExpDirL = REV;
		if(ExpSpeedR >= 0) ExpDirR = FWD; else ExpDirR = REV;
		SetRunningDir(ExpDirL, ExpDirR);
		SetRunningSpeed((uint16_t)ABS(ExpSpeedL), (uint16_t)ABS(ExpSpeedR));
	}

	_ctrl_ticks ++;
	if(_ctrl_ticks >= 60000)
		_ctrl_ticks = 0;
}

static uint16_t StableTimeCnt = 0;
static int16_t old_gx = 0, old_gy = 0, old_gz = 0;
/*
 * IMU stabled check task.
 */
static uint8_t IMU_StableCheck(void)
{
	uint8_t ret = 0;
	if((ABS(pMPU->gyrX - old_gx) + ABS(pMPU->gyrY - old_gy) + ABS(pMPU->gyrZ - old_gz)) < 5) {
		if(StableTimeCnt < 200)
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

uint8_t IMU_GotOffset(void)
{
	return IMU_Stabled;
}

uint8_t GetSignalLostFlag(void)
{
	return SignalLostFlag;
}

uint8_t GetVehicleRunState(void)
{
	return RunEnableFlag;
}
