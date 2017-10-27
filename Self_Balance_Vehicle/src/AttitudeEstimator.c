#include "AttitudeEstimator.h"

static AccDataDef *pAcc;
static GyrDataDef *pGyr;
static Quaternion AttitudeQuat = {1, 0, 0, 0};
static EulerAngle AttitudeAngle = {0, 0, 0};

void EstimatorInit(void)
{
	pAcc = GetAccDataPointer();
	pGyr = GetGyrDataPointer();
}

void FusionIMU_6Axis(float dt)
{
	float recipNorm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;

	float twoKp = 1.0f;    // 2 * proportional gain (Kp)
	float twoKi = 0.0f;    // 2 * integral gain (Ki)

	static float integralFBx = 0.0f;
	static float integralFBy = 0.0f;
	static float integralFBz = 0.0f;  // integral error terms scaled by Ki

	float qw, qx, qy, qz;
	float gx, gy, gz;
	float ax, ay, az;

	float delta2;

	float qw_last = AttitudeQuat.qw;
	float qx_last = AttitudeQuat.qx;
	float qy_last = AttitudeQuat.qy;
	float qz_last = AttitudeQuat.qz;

	gx = pGyr->gyrX * DEG_TO_RAD;
	gy = pGyr->gyrY * DEG_TO_RAD;
	gz = pGyr->gyrZ * DEG_TO_RAD;

	ax = pAcc->accX;
	ay = pAcc->accY;
	az = pAcc->accZ;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	{
		// Normalise accelerometer measurement
		recipNorm = 1.0f/sqrtf(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = qx_last * qz_last - qw_last * qy_last;
		halfvy = qw_last * qx_last + qy_last * qz_last;
		halfvz = qw_last * qw_last - 0.5f + qz_last * qz_last;

		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled
		integralFBx += twoKi * halfex * dt;  // integral error scaled by Ki
		integralFBy += twoKi * halfey * dt;
		integralFBz += twoKi * halfez * dt;
		gx += integralFBx;  // apply integral feedback
		gy += integralFBy;
		gz += integralFBz;

		// Apply proportional feedback
		gx += twoKp * halfex;
		gy += twoKp * halfey;
		gz += twoKp * halfez;
	}

	delta2 = (gx*gx + gy*gy + gz*gz)*dt*dt;

	qw = qw_last*(1.0f-delta2*0.125f) + (-qx_last*gx - qy_last*gy - qz_last*gz)*0.5f * dt;
	qx = qx_last*(1.0f-delta2*0.125f) + (qw_last*gx + qy_last*gz - qz_last*gy)*0.5f * dt;
	qy = qy_last*(1.0f-delta2*0.125f) + (qw_last*gy - qx_last*gz + qz_last*gx)*0.5f * dt;
	qz = qz_last*(1.0f-delta2*0.125f) + (qw_last*gz + qx_last*gy - qy_last*gx)*0.5f * dt;

	// Normalise quaternion
	recipNorm = 1.0f/sqrtf(qw * qw + qx * qx + qy * qy + qz * qz);
	qw *= recipNorm;
	qx *= recipNorm;
	qy *= recipNorm;
	qz *= recipNorm;

	AttitudeQuat.qw = qw;
	AttitudeQuat.qx = qx;
	AttitudeQuat.qy = qy;
	AttitudeQuat.qz = qz;

	Quat2Euler(&AttitudeQuat, &AttitudeAngle);
}

Quaternion *GetAttitudeQuate(void) {return &AttitudeQuat;}
EulerAngle *GetAttitudeAngle(void) {return &AttitudeAngle;}

void Quat2Euler(Quaternion* Q, EulerAngle* eur)
{
	float qw,qx,qy,qz;

	qw = Q->qw;
	qx = Q->qx;
	qy = Q->qy;
	qz = Q->qz;

	eur->roll  = atan2f(2 * (qw * qx + qy * qz) , 1 - 2 * (qx * qx + qy * qy)) * RAD_TO_DEG;
	eur->pitch = asinf(2 * (qw * qy - qz * qx)) * RAD_TO_DEG;
	eur->yaw   = atan2f(2 * (qw * qz + qx * qy) , 1 - 2 * (qy * qy + qz * qz)) * RAD_TO_DEG;
}
