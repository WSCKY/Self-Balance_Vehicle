#include "DataMonitor.h"

static uint8_t _init_flag = 0;
static COM_DATA_PACK_DEF ComDataBuf = {0};

static AccDataDef *pAcc;
static GyrDataDef *pGyr;
static EulerAngle *pEulerAngle;

void InitDataPackage(COMM_DATA *pComData)
{
	pComData->head = 0x55;
	pComData->cmd = 0xDD;
	pComData->length = 36;
	pComData->checksum = 0;
}

void DataPackageChkSum(COMM_DATA *pComData)
{
	uint8_t chk = 0, i = 0;
	for(; i < 36; i ++)
		chk += pComData->data[i / 4].c_data[i % 4];
	pComData->checksum = chk;
}

void SendDataToMonitor(void)
{
	if(_init_flag == 0) {
		_init_flag = 1;

		pAcc = GetAccDataPointer();
		pGyr = GetGyrDataPointer();
		pEulerAngle = GetAttitudeAngle();

		InitDataPackage(&ComDataBuf.ComData);
		return;
	}

	ComDataBuf.ComData.data[0].f_data = pAcc->accX;
	ComDataBuf.ComData.data[1].f_data = pAcc->accY;
	ComDataBuf.ComData.data[2].f_data = pAcc->accZ;
	ComDataBuf.ComData.data[3].f_data = pGyr->gyrX;
	ComDataBuf.ComData.data[4].f_data = pGyr->gyrY;
	ComDataBuf.ComData.data[5].f_data = pGyr->gyrZ;
	ComDataBuf.ComData.data[6].f_data = pEulerAngle->pitch;
	ComDataBuf.ComData.data[7].f_data = pEulerAngle->roll;
	ComDataBuf.ComData.data[8].f_data = pEulerAngle->yaw;
	DataPackageChkSum(&ComDataBuf.ComData);

	DebugPortSendBytesDMA(ComDataBuf.RawData, 40);
}
