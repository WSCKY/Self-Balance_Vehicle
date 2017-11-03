#include "RfCtrlMode.h"

static uint8_t _loop_time = 0;
static uint8_t _init_flag = 0;

static RC_CHANNLE_t *pRC;
static uint8_t SignalLostFlag = 1;
static uint32_t SignalLostCnt = 0;

static float TOF_Distance = 0.0f;
static uint8_t TOF_SignalLostFlag = 1;
static uint32_t TOF_SignalLostCnt = 0;

static float ExpVel = 0.0f, ExpYaw = 0.0f;

static void CtrlModeInit(void)
{
	_loop_time = 1000 / RF_CTRL_MODE_LOOP_RATE;

	SignalLostCnt = 0;
	SignalLostFlag = 1;
	pRC = GetRC_ChannelData();
}

void RfCtrlModeLoop(void)
{
	if(_init_flag == 0) {
		_init_flag = 1;

		CtrlModeInit();
	}

	if(GetRCUpdateFlag()) {
		SignalLostCnt = 0;
		SignalLostFlag = 0;
		RC_ParseData();
	} else {
		if(SignalLostCnt * _loop_time < RF_SIGNAL_LOST_CONFIRM_TIME)
			SignalLostCnt ++;
		else {
			SignalLostFlag = 1;
		}
	}

	if(GetNewTOFData(&TOF_Distance)) {
		TOF_SignalLostCnt = 0;
		TOF_SignalLostFlag = 0;
	} else {
		if(TOF_SignalLostCnt * _loop_time < TOF_SIGNAL_LOST_CONFIRM_TIME)
			TOF_SignalLostCnt ++;
		else {
			TOF_SignalLostFlag = 1;
		}
	}

	if(SignalLostFlag) {
		step_change(&ExpVel, 0, 0.4f, 0.4f);
		step_change(&ExpYaw, 0, 4.0f, 4.0f);
	} else {
		step_change(&ExpVel, apply_deadband(((RC_MIDDLE_VALUE - pRC->Channel[1]) * 2 / 35.0f), 4.0f), 0.4f, 0.4f);
		step_change(&ExpYaw, apply_deadband(((RC_MIDDLE_VALUE - pRC->Channel[0]) * 8 / 35.0f), 16.0f), 4.0f, 4.0f);
	}

	if(TOF_SignalLostFlag == 0) {
		if(TOF_Distance <= 45.0f) {
			
		} else if(TOF_Distance <= 60.0f) {
			
		}
	}

	SetUsrCtrlVal(ExpVel, ExpYaw);
}

uint8_t GetSignalLostFlag(void)
{
	return SignalLostFlag;
}
