/*
 * stepper.c
 *
 *  Created on: Apr 2, 2026
 *      Author: emmanuel
 */
#include "stepper.h"
#include "main.h"
/* ─────────────────────────────────────────────
   StepperEnable / StepperDisable
   A4988 EN pin is active-LOW on most boards.
   Adjust polarity if yours differs.
───────────────────────────────────────────── */
void StepperEnable(void)
{
	/* Active LOW: pull EN low to enable driver */
	HAL_GPIO_WritePin(ENA4988_1_GPIO_Port, ENA4988_1_Pin, GPIO_PIN_RESET);
}

void StepperDisable(void)
{
	HAL_GPIO_WritePin(ENA4988_1_GPIO_Port, ENA4988_1_Pin, GPIO_PIN_SET);
}

/* ─────────────────────────────────────────────
   MoveSteps
   milimeters > 0  →  forward  (encoder counts UP,   DIR = HIGH)
   milimeters < 0  →  backward (encoder counts DOWN,  DIR = LOW )
   milimeters = 0  →  MOVE_ERR_INVALID

   Returns MoveResult so the caller can react.
───────────────────────────────────────────── */
MoveResult MoveSteps(int32_t milimeters)
{
	/* ── 0. Reject zero movement ─────────────────────── */
	if (milimeters == 0) {
		return MOVE_ERR_INVALID;
	}

	/* ── 1. Convert mm → encoder counts (signed) ─────── */
	int32_t delta_counts = (int32_t)(milimeters * COUNTS_PER_MM);
	//  NOTE: fractional truncation here is intentional.
	//  At 23.66 counts/mm the rounding error is < 1 count (< 0.05 mm).
	//  If sub-count precision is needed later, accumulate remainder.

	/* ── 2. Read current encoder position ────────────── */
	int32_t act_counter = (int32_t)__HAL_TIM_GET_COUNTER(&htim5);

	/* ── 3. Compute intended target position ─────────── */
	int32_t target_counter = act_counter + delta_counts;

	/* ── 4. Security: check target against hard limits ── */
	//  We check the TARGET, not act_counter, so we catch over-travel
	//  before the motor even starts moving.
	if (target_counter > (int32_t)LIMIT_UPPER) {
		return MOVE_ERR_UPPER_LIMIT;   /* hard stop – motor never starts */
	}
	if (target_counter < (int32_t)LIMIT_LOWER) {
		return MOVE_ERR_LOWER_LIMIT;
	}

	/* ── 5. Set direction pin ────────────────────────── */
	if (milimeters > 0) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);   /* Forward */
	} else {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); /* Backward */
	}

	/* ── 6. Enable driver and start PWM ─────────────── */
	StepperEnable();
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

	/* ── 7. Poll encoder until target reached ────────── */
	//  We re-read the encoder every loop iteration.
	//  The motor pulses itself via PWM — we just watch.
	while (1)
	{
		act_counter = (int32_t)__HAL_TIM_GET_COUNTER(&htim5);

		/* ── 7a. In-motion limit safety re-check ──────
           Guards against mechanical slip or missed counts
           pushing us past the limit mid-move.             */
		if (act_counter >= (int32_t)LIMIT_UPPER) {
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			StepperDisable();
			return MOVE_ERR_UPPER_LIMIT;
		}
		if (act_counter <= (int32_t)LIMIT_LOWER) {
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
			StepperDisable();
			return MOVE_ERR_LOWER_LIMIT;
		}

		/* ── 7b. Target reached? ───────────────────── */
		int8_t reached;
		if (milimeters > 0) {
			reached = (act_counter >= target_counter);
		} else {
			reached = (act_counter <= target_counter);
		}

		if (reached) {
			break;
		}
	}

	/* ── 8. Stop PWM and disable driver ─────────────── */
	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
	StepperDisable();

	return MOVE_OK;
}

