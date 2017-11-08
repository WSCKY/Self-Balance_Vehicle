#ifndef __YAWCONTROLLER_H
#define __YAWCONTROLLER_H

#include "SysConfig.h"

#include "pidController.h"
#include "AttitudeEstimator.h"

#define YAW_CTRL_RATE_DIV              (2)

void YawControlLoop(float ExpYawRate, uint8_t ControllerEnable);
float GetYawControllerOutput(void);

#endif /* __YAWCONTROLLER_H */
