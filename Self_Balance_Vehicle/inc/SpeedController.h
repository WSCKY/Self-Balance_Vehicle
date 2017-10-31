#ifndef __SPEEDCONTROLLER_H
#define __SPEEDCONTROLLER_H

#include "Encoder.h"
#include "pidController.h"

#define SpeedReadDiv                   (2)

void SpeedComputeTask(void);
void SpeedControlLoop(float ExpVel, uint8_t ControllerEnable);
float GetSpeedControllerOutput(void);

#endif /* __SPEEDCONTROLLER_H */
