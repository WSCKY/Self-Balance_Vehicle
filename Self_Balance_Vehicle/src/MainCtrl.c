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

static float ExpSpeedL = 0, ExpSpeedR = 0;
static TURN_DIR ExpDirL = STOP, ExpDirR = STOP;
static float CtrlModeExpVel = 0.0f, CtrlModeExpYaw = 0.0f;

/* private function prototype. */
static uint8_t IMU_StableCheck(void);

static void MainCtrlLoopInit(void)
{
	MainCtrlLoopDt = 1.0f / SYSTEM_LOOP_RATE;
	pMPU = GetMPU_RawDataPointer();
	pEulerAngle = GetAttitudeAngle();
}

void SystemControlTask(void) /* SYSTEM_LOOP_RATE Hz */
{
	if(_init_flag == 0) {
		_init_flag = 1;

		MainCtrlLoopInit();
	}
	MPU6050_Read(&GyrOffset, IMU_Stabled);

	SpeedComputeTask();

	if(!IMU_Stabled)
		IMU_StableCheck();

	if(IMU_Stabled) {
		FusionIMU_6Axis(MainCtrlLoopDt);
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

	RfCtrlModeLoop();

	AttitudeControlLoop(-1.8f, RunEnableFlag);
	SpeedControlLoop(CtrlModeExpVel, RunEnableFlag);
	YawControlLoop(CtrlModeExpYaw, RunEnableFlag);

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

uint8_t GetVehicleRunState(void)
{
	return RunEnableFlag;
}

void SetUsrCtrlVal(float ExpVel, float ExpYaw)
{
	CtrlModeExpVel = ExpVel;
	CtrlModeExpYaw = ExpYaw;
}
