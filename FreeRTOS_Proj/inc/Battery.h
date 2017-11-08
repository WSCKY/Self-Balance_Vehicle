#ifndef __BATTERY_H
#define __BATTERY_H

#include "stm32f10x_adc.h"

void VoltageMeasureInit(void);
float GetSampleVoltage(void);

#endif /* __BATTERY_H */
