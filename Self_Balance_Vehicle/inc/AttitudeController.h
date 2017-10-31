#ifndef __ATTITUDECONTROLLER_H
#define __ATTITUDECONTROLLER_H

#include "SysConfig.h"

#include "pidController.h"
#include "AttitudeEstimator.h"

#define ANGLE_CTRL_RATE_DIV            (2)

#define GYROS_CTRL_RATE_DIV            (1)

#if ((ANGLE_CTRL_RATE_DIV == 0) || (GYROS_CTRL_RATE_DIV == 0))
 #error "ERROR CONFIG: ATTITUDE RATE DIV"
#endif

void AttitudeControlLoop(float ExpAngle, uint8_t ControllerEnable);
void GetAttitudeControllerOutput(float *Left, float *Right);

#endif /* __ATTITUDECONTROLLER_H */
