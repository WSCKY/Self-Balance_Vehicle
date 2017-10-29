/* Includes */
#include "pidController.h"
#include <math.h>

/*
 * PI controller code.
 */
void pid_loop(PID* pid, float expect, float measure)
{
	float err = expect - measure;

	if(isnan(err)) return;

	pid->I_sum += err * pid->dt;
	if(pid->I_sum > pid->I_max )
		pid->I_sum = pid->I_max;
	if(pid->I_sum < -pid->I_max )
		pid->I_sum = -pid->I_max;

	pid->Pout = pid->kp * err;
	pid->Iout = pid->ki * pid->I_sum;

	pid->Output = pid->Pout + pid->Iout;
}

