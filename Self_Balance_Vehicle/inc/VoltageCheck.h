#ifndef __VOLTAGECHECK_H
#define __VOLTAGECHECK_H

#include "SysConfig.h"

#include "Battery.h"

#define VOLT_CHECK_LOOP_RATE_DIV       (4)
/* power state confirm time(in ticks) */
#define VOLT_STATE_CONFIRM_TICKS       (50)

typedef enum {
	PowerOK = 0,
	LowPower = 1,
	NoPower = 2,
	BadBattery = 3,
} POWER_STATE_DEF;

void VoltageCheckLoop(void);
float GetSystemVoltage(void);
POWER_STATE_DEF GetSystemPowerState(void);

#endif /* __VOLTAGECHECK_H */
