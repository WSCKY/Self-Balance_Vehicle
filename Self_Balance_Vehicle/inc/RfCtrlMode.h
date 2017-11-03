#ifndef __RFCTRLMODE_H
#define __RFCTRLMODE_H

#include "SysConfig.h"

#include "TOF.h"
#include "Maths.h"
#include "MainCtrl.h"
#include "RemoteController.h"

#define RF_CTRL_MODE_LOOP_RATE         SYSTEM_LOOP_RATE

/* signal lost confirm time in ms */
#define RF_SIGNAL_LOST_CONFIRM_TIME    (1000)
#define TOF_SIGNAL_LOST_CONFIRM_TIME   (1000)

void RfCtrlModeLoop(void);
uint8_t GetSignalLostFlag(void);

#endif /* __RFCTRLMODE_H */
