/*
 * mpu6000.h
 *
 *  Created on: Dec 2, 2025
 *      Author: emmanuel
 */

#ifndef INC_MPU6000_H_
#define INC_MPU6000_H_
/*=============================================*/
			/* PRIVATE INCLUDES */
/*=============================================*/
#include "main.h" //Include main.h to get the I2C_HandleTypeDef
#include "stm32f4xx_hal.h"
#include "stdint.h"
#include "stdio.h"
/*==============================================*/

/*=============================================*/
/* CONSTANTS AND REGISTERS OF THE ACCELEROMETER */
/*=============================================*/
#define MPU_ADDR_BASE_0X68 0xD0 //Register direction of the MPU connected to GND
#define MPU_WHO_AM_I_REG 0x75 //Register direction of the WHO_AM_I
#define MPU_WHO_AM_I_EXPECTED 0x68 //Register value expected of the WHO_AM_I
#define MPU_PWR_MGMT_1_REG 0x6B //Control of the power management, if the CYCLE bit turn to 1 and SLEEP to 0: Turns on -> Take Data -> Turns off
#define MPU_GYRO_CONFIG_REG 0x1B //Selects the scaale range of the maximum angular velocity that the sensor can test and turns on/off self test
#define MPU_ACCEL_CONFIG_REG 0x1C //Selects scale range of the maximum accelration gravity it can test and turns on/off the self-test
#define MPU_ACCEL_XOUT_H_REG 0x3B //Sets the initial address for the reading of the 6 axis (MPU reads the three axis data. Each axis gives a 16 bit data info, but as I2C can only read 8 bit at a time, then we have to read the data by half)
#define MPU_ACCEL_SENSITIVITY_2G    16384.0f //AFS_SEL -> 0 / Sensitivity = 16834 datasheet info 13 MPU-6000
/*=============================================*/

/*==============================================*/
				/*STRUCTURE DEFINES*/
/*==============================================*/
//The following struct is created so we can assigned the different register values as a state memory
typedef struct{
	I2C_HandleTypeDef *hi2c; //Pointer to the corresponding I2C bus selected
	uint8_t I2C_ADRRESS; //Slave Accelerometer's address
	float Accel_ScaleFactor; //Conversion factor for accelerations results
}MPU_HandleTypeDef;

/*==============================================*/
			  /*FUNCTION HEADERS*/
/*==============================================*/
HAL_StatusTypeDef MPU_Init(MPU_HandleTypeDef *hdev);
HAL_StatusTypeDef MPU_CheckID(MPU_HandleTypeDef *hdev);
HAL_StatusTypeDef MPU_ReadRaw(MPU_HandleTypeDef *hdev, int16_t *raw_x, int16_t *raw_y, int16_t *raw_z);
HAL_StatusTypeDef MPU_GetAcceleration(MPU_HandleTypeDef *hdev, float *accel_x, float *accel_y, float *accel_z);
HAL_StatusTypeDef MPU_WriteRegister(MPU_HandleTypeDef *hdev, uint8_t reg_addr, uint8_t data);
HAL_StatusTypeDef MPU_ReadRegister(MPU_HandleTypeDef *hdev, uint8_t reg_addr, uint8_t *rx_buffer, uint16_t len);

#endif /* INC_MPU6000_H_ */

