#include "mpu6050.h"

#define TIMEOUT_MAX             0x1000   /*<! The value of the maximal timeout for I2C waiting loops */
uint32_t I2C_TimeOut = TIMEOUT_MAX; /*<! Value of Timeout when I2C communication fails */

static MPU_RAW mpu_data;
static AccDataDef accVal;
static GyrDataDef gyrVal;
static float temperature = 0;
static uint8_t mpu_rx_buffer[14] = {0};

static void i2c_config(void);
static void mpu6050_config(void);

static uint8_t I2C_WriteDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t RegisterValue);
static uint8_t I2C_ReadDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr);
static uint8_t I2C_ReadDataBuffer(uint8_t DeviceAddr, uint32_t RegisterAddr, uint8_t *pBuffer, uint8_t len);
static void I2C_DMA_Config(DMADirection_TypeDef Direction, uint8_t* buffer, uint8_t len);
static uint8_t I2C_TimeoutUserCallback(void);

/*
 * Initialize MPU6050 sensor.
 */
void MPU6050_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIO Periph clock enable */
	RCC_APB2PeriphClockCmd(MPU6050_GPIO_PORT_CLK | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = MPU6050_GPIO_SCL_PIN | MPU6050_GPIO_SDA_PIN;
	GPIO_Init(MPU6050_GPIO_PORT, &GPIO_InitStructure);

	i2c_config();
	Delay(100);
	mpu6050_config();
}

/*
 * read mpu6500 once.
 */
void MPU6500_Read(void)
{
	I2C_ReadDataBuffer(MPU6050_DEVICE_ADDR, 0x3B, mpu_rx_buffer, 14);
	/* Reorganize received data */
	((uint8_t *)&mpu_data.accX)[1] = mpu_rx_buffer[0];
	((uint8_t *)&mpu_data.accX)[0] = mpu_rx_buffer[1];
	((uint8_t *)&mpu_data.accY)[1] = mpu_rx_buffer[2];
	((uint8_t *)&mpu_data.accY)[0] = mpu_rx_buffer[3];
	((uint8_t *)&mpu_data.accZ)[1] = mpu_rx_buffer[4];
	((uint8_t *)&mpu_data.accZ)[0] = mpu_rx_buffer[5];
	((uint8_t *)&mpu_data.temp)[1] = mpu_rx_buffer[6];
	((uint8_t *)&mpu_data.temp)[0] = mpu_rx_buffer[7];
	((uint8_t *)&mpu_data.gyrX)[1] = mpu_rx_buffer[8];
	((uint8_t *)&mpu_data.gyrX)[0] = mpu_rx_buffer[9];
	((uint8_t *)&mpu_data.gyrY)[1] = mpu_rx_buffer[10];
	((uint8_t *)&mpu_data.gyrY)[0] = mpu_rx_buffer[11];
	((uint8_t *)&mpu_data.gyrZ)[1] = mpu_rx_buffer[12];
	((uint8_t *)&mpu_data.gyrZ)[0] = mpu_rx_buffer[13];
	accVal.accX = mpu_data.accX * 0.002392578125f;
	accVal.accY = mpu_data.accY * 0.002392578125f;
	accVal.accZ = mpu_data.accZ * 0.002392578125f;
	temperature = (mpu_data.temp / 340.0f) + 36.53f;
	gyrVal.gyrX = mpu_data.gyrX * 0.06103515625f;
	gyrVal.gyrY = mpu_data.gyrY * 0.06103515625f;
	gyrVal.gyrZ = mpu_data.gyrZ * 0.06103515625f;
}

AccDataDef *GetAccDataPointer(void) {return &accVal;}
GyrDataDef *GetGyrDataPointer(void) {return &gyrVal;}
float GetMPU6050Temperature(void) {return temperature;}

uint8_t MPU6050_ID = 0; /* The default value is 0x68 */
/*
 * configure the MPU6050 registers.
 */
static void mpu6050_config(void)
{
	I2C_WriteDeviceRegister(MPU6050_DEVICE_ADDR, 0x6B, 0x03);
	I2C_WriteDeviceRegister(MPU6050_DEVICE_ADDR, 0x19, 0x00);
	I2C_WriteDeviceRegister(MPU6050_DEVICE_ADDR, 0x1A, 0x03);
	I2C_WriteDeviceRegister(MPU6050_DEVICE_ADDR, 0x1B, 0x03 << 3);
	I2C_WriteDeviceRegister(MPU6050_DEVICE_ADDR, 0x1C, 0x02 << 3);

	MPU6050_ID = I2C_ReadDeviceRegister(MPU6050_DEVICE_ADDR, 0x75);
}

/*
 * Configure the I2C Peripheral used to communicate with MPU6050.
 */
static void i2c_config(void)
{
	I2C_InitTypeDef I2C_InitStructure;
	/* MPU6050_I2C periph clock enable */
	RCC_APB1PeriphClockCmd(MPU6050_I2C_CLK, ENABLE);

	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_ClockSpeed = MPU6050_I2C_SPEED;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(MPU6050_I2C, &I2C_InitStructure);

	I2C_Cmd(MPU6050_I2C, ENABLE);
}

/**
  * @brief  Writes a value in a register of the device through I2C.
  * @param  DeviceAddr: The address of the MPU6050, could be : MPU6050_DEVICE_ADDR.
  * @param  RegisterAddr: The target register adress
  * @param  RegisterValue: The target register value to be written 
  * @retval 0: if all operations are OK. Other value if error.
  */
static uint8_t I2C_WriteDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t RegisterValue)
{
	uint32_t read_verif = 0;
	uint8_t I2C_BufferTX = 0;

	/* Get Value to be written */
	I2C_BufferTX = RegisterValue;

	/* Configure DMA Peripheral */
	I2C_DMA_Config(I2C_DMA_TX, (uint8_t *)(&I2C_BufferTX), 1);

	/* Enable the I2C peripheral */
	I2C_GenerateSTART(MPU6050_I2C, ENABLE);

  /* Test on SB Flag */
  I2C_TimeOut = TIMEOUT_MAX;
  while (I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_SB) == RESET) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
	}

	/* Transmit the slave address and enable writing operation */
	I2C_Send7bitAddress(MPU6050_I2C, DeviceAddr, I2C_Direction_Transmitter);

	/* Test on ADDR Flag */
	I2C_TimeOut = TIMEOUT_MAX;
	while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Transmit the first address for r/w operations */
  I2C_SendData(MPU6050_I2C, RegisterAddr);

  /* Test on TXE FLag (data dent) */
  I2C_TimeOut = TIMEOUT_MAX;
  while ((!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_BTF))) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

	/* Enable I2C DMA request */
	I2C_DMACmd(MPU6050_I2C, ENABLE);

	/* Enable DMA TX Channel */
	DMA_Cmd(MPU6050_I2C_DMA_TX_CHANNEL, ENABLE);

	/* Wait until DMA Transfer Complete */
	I2C_TimeOut = TIMEOUT_MAX;
  while (!DMA_GetFlagStatus(MPU6050_I2C_DMA_TX_TCFLAG)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Wait until BTF Flag is set before generating STOP */
  I2C_TimeOut = 2 * TIMEOUT_MAX;
  while ((!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_BTF))) {}

	/* Send STOP Condition */
	I2C_GenerateSTOP(MPU6050_I2C, ENABLE);

	/* Disable DMA TX Channel */
	DMA_Cmd(MPU6050_I2C_DMA_TX_CHANNEL, DISABLE);

	/* Disable I2C DMA request */
	I2C_DMACmd(MPU6050_I2C, DISABLE);

	/* Clear DMA TX Transfer Complete Flag */
	DMA_ClearFlag(MPU6050_I2C_DMA_TX_TCFLAG);

#ifdef VERIFY_WRITTENDATA
  /* Verify (if needed) that the loaded data is correct */

  /* Read the just written register*/
  read_verif = I2C_ReadDeviceRegister(DeviceAddr, RegisterAddr);
  /* Load the register and verify its value */
  if (read_verif != RegisterValue) {
    /* Control data wrongly tranfered */
    read_verif = IOE_FAILURE;
  } else {
    /* Control data correctly transfered */
    read_verif = 0;
  }
#endif

  /* Return the verifying value: 0 (Passed) or 1 (Failed) */
  return read_verif;
}

/**
  * @brief  Reads a register of the device through I2C.
  * @param  DeviceAddr: The address of the device, could be : MPU6050_DEVICE_ADDR.
  * @param  RegisterAddr: The target register adress.
  * @retval The value of the read register (0xFF if Timout occured)
  */
static uint8_t I2C_ReadDeviceRegister(uint8_t DeviceAddr, uint8_t RegisterAddr)
{
  uint8_t I2C_BufferRX[2] = {0x00, 0x00};

  /* Configure DMA Peripheral */
  I2C_DMA_Config(I2C_DMA_RX, (uint8_t*)I2C_BufferRX, 2);

  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(MPU6050_I2C, ENABLE);

  /* Enable the I2C peripheral */
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);

  /* Test on SB Flag */
  I2C_TimeOut = TIMEOUT_MAX;
  while (!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_SB)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Send device address for write */
  I2C_Send7bitAddress(MPU6050_I2C, DeviceAddr, I2C_Direction_Transmitter);

  /* Test on ADDR Flag */
  I2C_TimeOut = TIMEOUT_MAX;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Send the device's internal address to write to */
  I2C_SendData(MPU6050_I2C, RegisterAddr);

  /* Test on TXE FLag (data dent) */
  I2C_TimeOut = TIMEOUT_MAX;
  while ((!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_BTF))) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Send START condition a second time */
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);

  /* Test on SB Flag */
  I2C_TimeOut = TIMEOUT_MAX;
  while (!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_SB)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Send IOExpander address for read */
  I2C_Send7bitAddress(MPU6050_I2C, DeviceAddr, I2C_Direction_Receiver);

  /* Test on ADDR Flag */
  I2C_TimeOut = TIMEOUT_MAX;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Enable I2C DMA request */
  I2C_DMACmd(MPU6050_I2C, ENABLE);

  /* Enable DMA RX Channel */
  DMA_Cmd(MPU6050_I2C_DMA_RX_CHANNEL, ENABLE);

  /* Wait until DMA Transfer Complete */
  I2C_TimeOut = 2 * TIMEOUT_MAX;
  while (!DMA_GetFlagStatus(MPU6050_I2C_DMA_RX_TCFLAG)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Send STOP Condition */
  I2C_GenerateSTOP(MPU6050_I2C, ENABLE);

  /* Disable DMA RX Channel */
  DMA_Cmd(MPU6050_I2C_DMA_RX_CHANNEL, DISABLE);

  /* Disable I2C DMA request */
  I2C_DMACmd(MPU6050_I2C, DISABLE);

  /* Clear DMA RX Transfer Complete Flag */
  DMA_ClearFlag(MPU6050_I2C_DMA_RX_TCFLAG);

  /* return a pointer to the IOE_Buffer */
  return (uint8_t)I2C_BufferRX[0];
}

/**
  * @brief  Reads a buffer of 2 bytes from the device registers.
  * @param  DeviceAddr: The address of the device, could be : MPU6050_DEVICE_ADDR.
  * @param  RegisterAddr: The target register adress.
  * @param  pBuffer: A pointer to the buffer to store the returned bytes.
  * @param  len: specify the length to receive from i2c.
  * @retval 0: if all operations are OK. Other value if error.
  */
static uint8_t I2C_ReadDataBuffer(uint8_t DeviceAddr, uint32_t RegisterAddr, uint8_t *pBuffer, uint8_t len)
{
  /* Configure DMA Peripheral */
  I2C_DMA_Config(I2C_DMA_RX, (uint8_t*)pBuffer, len);

  /* Enable DMA NACK automatic generation */
  I2C_DMALastTransferCmd(MPU6050_I2C, ENABLE);

  /* Enable the I2C peripheral */
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);

  /* Test on SB Flag */
  I2C_TimeOut = TIMEOUT_MAX;
  while (!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_SB)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Send device address for write */
  I2C_Send7bitAddress(MPU6050_I2C, DeviceAddr, I2C_Direction_Transmitter);

  /* Test on ADDR Flag */
  I2C_TimeOut = TIMEOUT_MAX;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Send the device's internal address to write to */
  I2C_SendData(MPU6050_I2C, RegisterAddr);

  /* Test on TXE FLag (data dent) */
  I2C_TimeOut = TIMEOUT_MAX;
  while ((!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_BTF))) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Send START condition a second time */
  I2C_GenerateSTART(MPU6050_I2C, ENABLE);

  /* Test on SB Flag */
  I2C_TimeOut = TIMEOUT_MAX;
  while (!I2C_GetFlagStatus(MPU6050_I2C, I2C_FLAG_SB)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

  /* Send IOExpander address for read */
  I2C_Send7bitAddress(MPU6050_I2C, DeviceAddr, I2C_Direction_Receiver);

  /* Test on ADDR Flag */
  I2C_TimeOut = TIMEOUT_MAX;
  while (!I2C_CheckEvent(MPU6050_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
	}

	/* Enable I2C DMA request */
	I2C_DMACmd(MPU6050_I2C, ENABLE);

	/* Enable DMA RX Channel */
	DMA_Cmd(MPU6050_I2C_DMA_RX_CHANNEL, ENABLE);

	/* Wait until DMA Transfer Complete */
	I2C_TimeOut = 2 * TIMEOUT_MAX;
  while (!DMA_GetFlagStatus(MPU6050_I2C_DMA_RX_TCFLAG)) {
    if (I2C_TimeOut -- == 0) return (I2C_TimeoutUserCallback());
  }

	/* Send STOP Condition */
	I2C_GenerateSTOP(MPU6050_I2C, ENABLE);

	/* Disable DMA RX Channel */
	DMA_Cmd(MPU6050_I2C_DMA_RX_CHANNEL, DISABLE);

	/* Disable I2C DMA request */
	I2C_DMACmd(MPU6050_I2C, DISABLE);

	/* Clear DMA RX Transfer Complete Flag */
	DMA_ClearFlag(MPU6050_I2C_DMA_RX_TCFLAG);

	/* return a pointer to the IOE_Buffer */
	return 0;
}

/**
  * @brief  Configure the DMA Peripheral used to handle communication via I2C.
  * @param  None
  * @retval None
  */
static void I2C_DMA_Config(DMADirection_TypeDef Direction, uint8_t* buffer, uint8_t len)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(MPU6050_I2C_DMA_CLK, ENABLE);

	/* Initialize the DMA_PeripheralBaseAddr member */
	DMA_InitStructure.DMA_PeripheralBaseAddr = MPU6050_I2C_DR;
	/* Initialize the DMA_MemoryBaseAddr member */
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
	/* Initialize the DMA_PeripheralInc member */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	/* Initialize the DMA_MemoryInc member */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	/* Initialize the DMA_PeripheralDataSize member */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	/* Initialize the DMA_MemoryDataSize member */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	/* Initialize the DMA_Mode member */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	/* Initialize the DMA_Priority member */
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	/* Initialize the DMA_M2M member */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	/* If using DMA for Reception */
	if (Direction == I2C_DMA_RX) {
		/* Initialize the DMA_DIR member */
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

		/* Initialize the DMA_BufferSize member */
		DMA_InitStructure.DMA_BufferSize = len;

		DMA_DeInit(MPU6050_I2C_DMA_RX_CHANNEL);

		DMA_Init(MPU6050_I2C_DMA_RX_CHANNEL, &DMA_InitStructure);
	}
	/* If using DMA for Transmission */
	else if (Direction == I2C_DMA_TX) {
		/* Initialize the DMA_DIR member */
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;

		/* Initialize the DMA_BufferSize member */
		DMA_InitStructure.DMA_BufferSize = len;

		DMA_DeInit(MPU6050_I2C_DMA_TX_CHANNEL);

		DMA_Init(MPU6050_I2C_DMA_TX_CHANNEL, &DMA_InitStructure);
	}
}

static uint8_t I2C_TimeoutUserCallback(void)
{
	return 0xFF;
}
