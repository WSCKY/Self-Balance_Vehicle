#ifndef __ATTITUDECONTROLLER_H
#define __ATTITUDECONTROLLER_H

#include "pidController.h"
#include "AttitudeEstimator.h"

void AttitudeControlLoop(uint8_t ExpAngle, uint8_t ControllerEnable);
void GetAttitudeControllerOutput(float *Left, float *Right);

#endif /* __ATTITUDECONTROLLER_H */
