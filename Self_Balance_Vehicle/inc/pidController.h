#ifndef __PIDCONTROLLER_H
#define __PIDCONTROLLER_H

#include "stm32f10x.h"

/* Public Definition */
typedef struct 
{
	float kp, ki, Pout, Iout, Output, I_max, I_sum, dt;
} PID;

/* Function Prototype */
void pid_loop(PID* pid, float expect, float measure);

#endif /* __PIDCONTROLLER_H */
