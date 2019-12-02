/*
 * ses_rotary.c
 *
 *  Created on: Jun 26, 2019
 *      Author: shady
 */

#include "ses_rotary.h"

//call-back functions for each direction
volatile pTypeRotaryCallback clockwise_callback = NULL;
volatile pTypeRotaryCallback counter_clockwise_callback = NULL;

void rotary_init(bool debouncing) {

	/*
	 * Configuring the pin of each direction of rotation as an input and
	 * activating the internal pull-up resistor for each direction of rotation
	 */
	DDR_REGISTER(A_ROTARY_PORT) &= ~(1 << A_ROTARY_PIN);
	DDR_REGISTER(B_ROTARY_PORT) &= ~(1 << B_ROTARY_PIN);
	A_ROTARY_PORT |= (1 << A_ROTARY_PIN);
	B_ROTARY_PORT |= (1 << B_ROTARY_PIN);

	/*checking the bouncing state of the button*/
	if (debouncing) {
		timer1_start();
		timer1_setCallback(rotary_checkState);
	}

}
bool rotary_isClockwiseRotated() {

	/* If the button is rotated , */
	if (PIN_REGISTER(A_ROTARY_PORT) & (1 << A_ROTARY_PIN)) {
		return false;
	} else {
		return true;
	}
}
bool rotary_isCounterClockwiseRotated() {

	if (PIN_REGISTER(B_ROTARY_PORT) & (1 << B_ROTARY_PIN)) {
		return false;
	} else {
		return true;
	}
}
void rotary_setClockwiseCallback(pTypeRotaryCallback callback) {

	if (callback != NULL) {
		clockwise_callback = callback;
	}

}
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback callback) {

	if (callback != NULL) {
		counter_clockwise_callback = callback;
	}

}
void rotary_checkState() {

	static uint8_t state[ROTARY_NUM_DEBOUNCE_CHECKS] = { };
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;
	// each bit in every state byte represents one button
	state[index] = 0;
	if (rotary_isClockwiseRotated()) {
		state[index] |= 1;
	}
	if (rotary_isCounterClockwiseRotated()) {
		state[index] |= 2;
	}
	index++;
	if (index == ROTARY_NUM_DEBOUNCE_CHECKS) {
		index = 0;
	}
	// init compare value and compare with ALL reads, only if
	// we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1" in the state
	// array, the button at this position is considered pressed
	uint8_t j = 0xFF;
	for (uint8_t i = 0; i < ROTARY_NUM_DEBOUNCE_CHECKS; i++) {
		j = j & state[i];
	}
	debouncedState = j;

	if ((debouncedState == 1 && lastDebouncedState == 0)
			&& (lastDebouncedState != debouncedState))
		clockwise_callback();

	else if ((debouncedState == 2 && lastDebouncedState == 0)
			&& (lastDebouncedState != debouncedState))
		counter_clockwise_callback();

}
