#ifndef __DATAMONITOR_H
#define __DATAMONITOR_H

#include "Debug.h"
#include "AttitudeEstimator.h"

__packed typedef struct
{
	uint8_t head;
	uint8_t cmd;
	uint8_t length;
	__packed union{
			uint8_t c_data[4];
			float f_data;	
	}data[9];
	uint8_t checksum;
} COMM_DATA;

__packed typedef union {
	COMM_DATA ComData;
	uint8_t RawData[40];
} COM_DATA_PACK_DEF;

void SendDataToMonitor(void);

#endif /* __DATAMONITOR_H */
