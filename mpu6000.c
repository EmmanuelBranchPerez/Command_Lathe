/*
 * mpu6000.c
 *
 *  Created on: Dec 2, 2025
 *      Author: emmanuel
 */

/*==============================================*/
/* PRIVATE INCLUDES */
/*==============================================*/
#include "mpu6000.h"
/*==============================================*/

/*==============================================*/
/* FUNCTION HANDLER */
/*==============================================*/
//Why are we using a HAL_StatusTypeDef? Basically here the Hal status tell's us if the i2c transaction has been successful
HAL_StatusTypeDef MPU_WriteRegister(MPU_HandleTypeDef *hdev, uint8_t reg_addr, uint8_t data)
{
	uint8_t tx_buffer[2] = {reg_addr, data}; //ChatGPT said:Here, we create a buffer because we need to send both the data and the register address of the device, unlike the LCD, which simply interprets commands

	return HAL_I2C_Master_Transmit(hdev->hi2c, hdev->I2C_ADRRESS, tx_buffer, 2, 100);//We return the HAL_I2C answer with: the pointer to the specific i2c channel, the address, the buffer, we specify that are 2 elements, and put a simple timeout
}

//Read Register first getting which is its direction and then receive the data and puts it into the sécific buffer created on the function that calls this header
HAL_StatusTypeDef MPU_ReadRegister(MPU_HandleTypeDef *hdev, uint8_t reg_addr, uint8_t *rx_buffer, uint16_t len)
{
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(hdev->hi2c, hdev->I2C_ADRRESS, &reg_addr, 1, 100);

	if (status != HAL_OK) return status;

	return HAL_I2C_Master_Receive(hdev->hi2c, hdev->I2C_ADRRESS, rx_buffer, len, 100);
}

//Verification of the device address
HAL_StatusTypeDef MPU_CheckID(MPU_HandleTypeDef *hdev)
{
	HAL_StatusTypeDef status;
	uint8_t id_leido;

	status = MPU_ReadRegister(hdev, MPU_WHO_AM_I_REG, &id_leido, 1);

	if (status != HAL_OK) {
		return status;
	}

	if (id_leido == MPU_WHO_AM_I_EXPECTED) {
		return HAL_OK;
	}
	else {
		return HAL_ERROR;
	}
}

//MPU Init
HAL_StatusTypeDef MPU_Init(MPU_HandleTypeDef *hdev)
{
	HAL_StatusTypeDef status;

	//Turn off sleep mode in the power management
	status = MPU_WriteRegister(hdev, MPU_PWR_MGMT_1_REG, 0x00);
	if (status != HAL_OK) return status;

	HAL_Delay(50);

	//+/- 250 deg/s (Bits 4-3 = 00)
	status = MPU_WriteRegister(hdev, MPU_GYRO_CONFIG_REG, 0x00);
	if (status != HAL_OK) return status;

	//+/- 2g (Bits 4-3 = 00)
	status = MPU_WriteRegister(hdev, MPU_ACCEL_CONFIG_REG, 0x00);
	if (status != HAL_OK) return status;

	//Save ScaleFactor
	hdev->Accel_ScaleFactor = MPU_ACCEL_SENSITIVITY_2G;

	//Everything was written successfully
	return HAL_OK;
}

//Take all de corresponding data from the different axis
HAL_StatusTypeDef MPU_ReadRaw(MPU_HandleTypeDef *hdev, int16_t *raw_x, int16_t *raw_y, int16_t *raw_z)
{
	uint8_t buffer_rx[6]; //buffer for the 6 axis info
	HAL_StatusTypeDef status;

	//Start the reading at MPU_ACCEL_XOUT_H_REG (0x3B)
	//MPU self-increments to continue the reading
	status = MPU_ReadRegister(hdev, MPU_ACCEL_XOUT_H_REG, buffer_rx, 6);

	if (status != HAL_OK) return status;

	//As I2C communication only reads 8 bit info, then we got to collect that info and join it to form the 16 bit info

	//X: [0] H [1] L
	*raw_x = (int16_t)(buffer_rx[0] << 8 | buffer_rx[1]);

	//Y: [2] H, [3] L
	*raw_y = (int16_t)(buffer_rx[2] << 8 | buffer_rx[3]);

	//Z: [4] H, [5] L
	*raw_z = (int16_t)(buffer_rx[4] << 8 | buffer_rx[5]);

	return HAL_OK;
}
//Get the acceleration
HAL_StatusTypeDef MPU_GetAcceleration(MPU_HandleTypeDef *hdev, float *accel_x, float *accel_y, float *accel_z)
{
	//Raw data saved
	int16_t raw_x, raw_y, raw_z;
	HAL_StatusTypeDef status;

	//Reading Raw data
	status = MPU_ReadRaw(hdev, &raw_x, &raw_y, &raw_z);

	if (status != HAL_OK) {
		return status; // Returns error status on failure
	}

	//Apply Scale Factor
	//Accel formula: accel = raw data/scale factor
	*accel_x = (float)raw_x / hdev->Accel_ScaleFactor;
	*accel_y = (float)raw_y / hdev->Accel_ScaleFactor;
	*accel_z = (float)raw_z / hdev->Accel_ScaleFactor;

	return HAL_OK;
}
/*==============================================*/

