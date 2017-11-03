#ifndef __MATHS_H
#define __MATHS_H

#include <math.h>
#include <stdint.h>

float apply_deadband(float value, float deadband);
void step_change(float *in, float target, float step, float deadBand);

#endif /* __MATHS_H */
