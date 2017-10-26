#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"

#include "Delay.h"

#define MPU6050_GPIO_PORT              GPIOB
#define MPU6050_GPIO_PORT_CLK          RCC_APB2Periph_GPIOB

#define MPU6050_GPIO_SCL_PIN           GPIO_Pin_8
#define MPU6050_GPIO_SDA_PIN           GPIO_Pin_9

#define MPU6050_I2C                    I2C1
#define MPU6050_I2C_CLK                RCC_APB1Periph_I2C1
#define MPU6050_I2C_DR                 ((uint32_t)0x40005410)
#define MPU6050_I2C_SPEED              (400000)

#define MPU6050_I2C_DMA                DMA1
#define MPU6050_I2C_DMA_CLK            RCC_AHBPeriph_DMA1

#define MPU6050_I2C_DMA_TX_CHANNEL     DMA1_Channel6
#define MPU6050_I2C_DMA_RX_CHANNEL     DMA1_Channel7
#define MPU6050_I2C_DMA_TX_TCFLAG      DMA1_FLAG_TC6
#define MPU6050_I2C_DMA_RX_TCFLAG      DMA1_FLAG_TC7

typedef enum
{
  I2C_DMA_TX = 0,
  I2C_DMA_RX = 1
} DMADirection_TypeDef;

typedef struct {
	int16_t accX;
	int16_t accY;
	int16_t accZ;
	int16_t temp;
	int16_t gyrX;
	int16_t gyrY;
	int16_t gyrZ;
} MPU_RAW;

typedef struct {
	int16_t accX;
	int16_t accY;
	int16_t accZ;
} AccRawDef;

typedef struct {
	int16_t gyrX;
	int16_t gyrY;
	int16_t gyrZ;
} GyrRawDef;

typedef struct {
	float accX;
	float accY;
	float accZ;
} AccDataDef;

typedef struct {
	float gyrX;
	float gyrY;
	float gyrZ;
} GyrDataDef;

#define MPU6050_DEVICE_ADDR            0xD0

void MPU6050_Init(void);
void MPU6500_Read(void);
AccDataDef *GetAccDataPointer(void);
GyrDataDef *GetGyrDataPointer(void);
float GetMPU6050Temperature(void);

#endif /* __MPU6050_H */
