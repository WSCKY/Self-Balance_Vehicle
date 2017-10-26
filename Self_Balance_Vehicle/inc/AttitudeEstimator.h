#ifndef __ATTITUDEESTIMATOR_H
#define __ATTITUDEESTIMATOR_H

#include "stm32f10x.h"

#include "mpu6050.h"
#include <math.h>

typedef struct
{
	float qw;
	float qx;
	float qy;
	float qz;
} Quaternion;

typedef struct 
{
	float roll;
	float pitch;
	float yaw;
} EulerAngle;

#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f

void EstimatorInit(void);
void FusionIMU_6Axis(float dt);
Quaternion *GetAttitudeQuate(void);
EulerAngle *GetAttitudeAngle(void);
void Quat2Euler(Quaternion* Q, EulerAngle* eur);

#endif /* __ATTITUDEESTIMATOR_H */
