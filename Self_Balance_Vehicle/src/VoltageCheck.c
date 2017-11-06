#include "VoltageCheck.h"

static uint16_t ControllerTicks = 0;

static float BatteryVolt = 12.0f;
static uint16_t BatteryNoPowerTimeCnt = 0;
static uint16_t BatteryLowPowerTimeCnt = 0;
static POWER_STATE_DEF BatteryPowerState = PowerOK;

void VoltageCheckLoop(void)
{
	if(ControllerTicks % VOLT_CHECK_LOOP_RATE_DIV == 0) {
		BatteryVolt = ComputeBatteryVoltage() * 0.02f + BatteryVolt * 0.98f;

		if(BatteryVolt < BATTERY_NO_POWER_LIMIT) {
			if(BatteryNoPowerTimeCnt < VOLT_STATE_CONFIRM_TICKS) {
				BatteryNoPowerTimeCnt ++;
			} else {
				BatteryPowerState = NoPower;
			}
		} else if(BatteryVolt < BATTERY_LOW_POWER_LIMIT) {
			if(BatteryLowPowerTimeCnt < VOLT_STATE_CONFIRM_TICKS) {
				BatteryLowPowerTimeCnt ++;
			} else {
				BatteryPowerState = LowPower;
			}
		} else {
			BatteryNoPowerTimeCnt = 0;
			BatteryLowPowerTimeCnt = 0;
		}
	}

	ControllerTicks ++;
	if(ControllerTicks >= 60000)
		ControllerTicks = 0;
}

float GetSystemVoltage(void)
{
	return BatteryVolt;
}

POWER_STATE_DEF GetSystemPowerState(void)
{
	return BatteryPowerState;
}
