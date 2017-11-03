#include "Maths.h"

float apply_deadband(float value, float deadband)
{
	if(fabs(value) <= deadband) {value = 0;}

	if(value > deadband) {value -= deadband;}
	if(value < -deadband) {value += deadband;}

	return value;
}

void step_change(float *in, float target, float step, float deadBand)
{
	if(fabsf(*in - target) <= deadBand) {*in = target; return;}

	if(*in > target) {*in -= fabsf(step); return;}
	if(*in < target) {*in += fabsf(step); return;}
}
