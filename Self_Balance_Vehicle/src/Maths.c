#include "Maths.h"

void step_change(float *in, float target, float step, float deadBand)
{
	if(fabsf(*in - target) <= deadBand) {*in = target; return;}

	if(*in > target) {*in -= fabsf(step); return;}
	if(*in < target) {*in += fabsf(step); return;}
}
