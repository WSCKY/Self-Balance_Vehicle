#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

/* main controller loop rate (in Hz) */
#define MAIN_CONTROLLER_LOOP_RATE      (200)

/* LED flash rate define */
#define LED_FLASH_RATE_WAIT_STABLE     (10)
#define LED_FLASH_RATE_SIGNAL_LOST     (2)

#define DEBUG_DATA_FRAME_RATE          (100)

/* Battery Low Power Limit */
#define BATTERY_NO_POWER_LIMIT         (11.0f)
#define BATTERY_LOW_POWER_LIMIT        (11.2f)

#endif /* __SYSCONFIG_H */
