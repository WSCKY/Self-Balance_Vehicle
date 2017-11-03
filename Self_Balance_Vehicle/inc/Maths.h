#ifndef __MATHS_H
#define __MATHS_H

#include <math.h>
#include <stdint.h>

#define LIMIT_MAX(x, y)                (((x) > (y)) ? (y) : (x))
#define LIMIT_MIN(x, y)                (((x) < (y)) ? (y) : (x))
#define LIMIT_RANGE(x, max, min)       (LIMIT_MAX(LIMIT_MIN((x), (min)), (max)))

float apply_deadband(float value, float deadband);
void step_change(float *in, float target, float step, float deadBand);

#endif /* __MATHS_H */
