/*
 * stepper.c
 *
 *  Created on: Apr 2, 2026
 *      Author: emmanuel
 */
#include "stepper.h"
#include "main.h"
/*==========================================*/
		/* STEPPER.C DOCUMENTATION */
/*==========================================*/

/*
 * The stepper.c main helps the lathe enable/disable
 * the PWM of the longitudinal stepper (future trans-
 * versal too. Both StepperEnable/StepperDisable help
 * stop or activate the lathe movement. Movesteps is
 * the specific part of the code that converts the u-
 * ser input into the specific pulses that the encoder
 * needs to detect.
 *
 * MoveSteps
   milimeters > 0  →  forward  (encoder counts UP,   DIR = HIGH)
   milimeters < 0  →  backward (encoder counts DOWN,  DIR = LOW )
   milimeters = 0  →  MOVE_ERR_INVALID

   Returns MoveResult so the caller can react.
 */

extern int32_t act_counter;

//Activate PWM for stepper
void StepperEnable(void)
{
	HAL_GPIO_WritePin(ENA4988_1_GPIO_Port, ENA4988_1_Pin, GPIO_PIN_RESET);//Enable in 0
}
//Deactivate PWM
void StepperDisable(void)
{
	HAL_GPIO_WritePin(ENA4988_1_GPIO_Port, ENA4988_1_Pin, GPIO_PIN_SET);//Disable in 1
}

MoveResult MoveSteps(int32_t milimeters)
{
	if (milimeters == 0) {
		return MOVE_ERR_INVALID;
	}

	// Convert mm → encoder counts (signed)
	int32_t delta_counts = (int32_t)(milimeters * COUNTS_PER_MM);//signed so the counter can get negative values

	// Counter position
	//int32_t act_counter = (int32_t)__HAL_TIM_GET_COUNTER(&htim5);
	act_counter = (int32_t)__HAL_TIM_GET_COUNTER(&htim5);

	// Counter to get
	int32_t target_counter = act_counter + delta_counts;

	// Security check !!!
	if (target_counter > (int32_t)LIMIT_UPPER) {
		return MOVE_ERR_UPPER_LIMIT; //printf("ERROR: Upper limit reached. Reconfigure position.\r\n");
	}
	if (target_counter < (int32_t)LIMIT_LOWER) {
		return MOVE_ERR_LOWER_LIMIT; //printf("ERROR: Lower limit reached. Reconfigure position.\r\n");
	}

	// DIR software active
	if (milimeters > 0) {
		HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);   /* Forward */
	} else {
		HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_SET); /* Backward */
	}

	// Call enable stepper func
	StepperEnable();
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

	// while loop for iteratively check counter
	while (1)
	{
		act_counter = (int32_t)__HAL_TIM_GET_COUNTER(&htim5); // counter get

		// Security check!!!
		if (act_counter >= (int32_t)LIMIT_UPPER) {
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			StepperDisable();
			return MOVE_ERR_UPPER_LIMIT;
		}
		if (act_counter < (int32_t)LIMIT_LOWER) {
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			StepperDisable();
			return MOVE_ERR_LOWER_LIMIT;
		}

		// False/True reached
		int8_t reached;
		if (milimeters > 0) {
			reached = (act_counter >= target_counter); //If act counter is equal to target_counter (True)
			__NOP();
		} else {
			reached = (act_counter <= target_counter); //If act still lower false
			__NOP();
		}

		if (reached) {
			__NOP();
			break;
		}
	}

	//PWM stop
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	StepperDisable();

	return MOVE_OK;
}

