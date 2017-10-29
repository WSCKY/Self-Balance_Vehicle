#ifndef __ATTITUDECONTROLLER_H
#define __ATTITUDECONTROLLER_H

#include "pidController.h"
#include "AttitudeEstimator.h"

void AttitudeControlLoop(float ExpAngle, uint8_t ControllerEnable);
void GetAttitudeControllerOutput(float *Left, float *Right);

#endif /* __ATTITUDECONTROLLER_H */
