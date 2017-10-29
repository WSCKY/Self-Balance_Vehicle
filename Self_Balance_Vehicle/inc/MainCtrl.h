#ifndef __MAINCTRL_H
#define __MAINCTRL_H

#include "Button.h"
#include "Encoder.h"
#include "TB6612FNG.h"
#include "pidController.h"
#include "AttitudeEstimator.h"
#include "AttitudeController.h"

#define ABS(x) ((x) >= 0 ? (x) : -(x))

#define RUN_ENABLE_CONFIRM             1000
#define RUN_DISABLE_CONFIRM            100

#endif /* __MAINCTRL_H */
