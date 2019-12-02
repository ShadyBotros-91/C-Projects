/*
 * ses_button.c
 *
 *  Created on: May 14, 2019
 *      Author: Abdallah ALTalmas & Shady Botros
 */

#include "ses_button.h"

//call-back functions for each button
volatile pButtonCallback rotary_callback = NULL;
volatile pButtonCallback joy_stick_callback = NULL;

//Interrupt service routine for each button
ISR(PCINT0_vect) {

	if ((button_isJoystickPressed()) && (joy_stick_callback != NULL)
			&& (PCMSK0 & (1 << BUTTON_JOYSTICK_PIN))) {

		joy_stick_callback();

	}
	if ((button_isRotaryPressed()) && (rotary_callback != NULL)
			&& (PCMSK0 & (1 << BUTTON_ROTARY_ENCODER_PIN))) {
		rotary_callback();

	}

}
void button_init(bool debouncing) {

	/*
	 * Configuring the pin of each button as an input and
	 * activating the internal pull-up resistor for each button
	 */
	DDR_REGISTER(BUTTON_PORT) &= ~((1 << BUTTON_ROTARY_ENCODER_PIN)
			| (1 << BUTTON_JOYSTICK_PIN));
	BUTTON_PORT |= ((1 << BUTTON_ROTARY_ENCODER_PIN)
			| (1 << BUTTON_JOYSTICK_PIN));

	if (debouncing) {
		timer1_start();
		timer1_setCallback(button_checkState);
	}

	else {
		/*
		 *activate the pin change interrupt by enabling the Pin Change Interrupt Enable 0 bit
		 *in the Pin Change Interrupt Control Register
		 *

		PCICR |= (1 << PCIE0);


		 *enable triggering an interrupt if a button is pressed by enabling the corresponding bit in the
		 *Pin Change Mask Register


		PCMSK0 |= (1 << BUTTON_ROTARY_ENCODER_PIN) | (1 << BUTTON_JOYSTICK_PIN);


		 * Globally enable all interrupts

		sei();*/
	}
}

/**
 * Get the state of the joy-stick button.
 */
bool button_isJoystickPressed(void) {

	if (PIN_REGISTER(BUTTON_PORT) & (1 << BUTTON_JOYSTICK_PIN)) {
		return false;
	} else {
		return true;
	}
}

/**
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void) {
	if (PIN_REGISTER(BUTTON_PORT) & (1 << BUTTON_ROTARY_ENCODER_PIN)) {
		return false;
	} else {
		return true;
	}
}

//Setting the call back function for rotary button
void button_setRotaryButtonCallback(pButtonCallback callback) {

	if (callback != NULL) {

		rotary_callback = callback;
	}
}

//Setting the call back function for the joy stick button
void button_setJoystickButtonCallback(pButtonCallback callback) {

	if (callback != NULL) {

		joy_stick_callback = callback;
	}

}

//Maintaining a function to avoid the bouncing of the buttons
void button_checkState() {

	static uint8_t state[BUTTON_NUM_DEBOUNCE_CHECKS] = { };
	static uint8_t index = 0;
	static uint8_t debouncedState = 0;
	uint8_t lastDebouncedState = debouncedState;
// each bit in every state byte represents one button
	state[index] = 0;
	if (button_isJoystickPressed()) {
		state[index] |= 1;
	}
	if (button_isRotaryPressed()) {
		state[index] |= 2;
	}
	index++;
	if (index == BUTTON_NUM_DEBOUNCE_CHECKS) {
		index = 0;
	}
// init compare value and compare with ALL reads, only if
// we read BUTTON_NUM_DEBOUNCE_CHECKS consistent "1" in the state
// array, the button at this position is considered pressed
	uint8_t j = 0xFF;
	for (uint8_t i = 0; i < BUTTON_NUM_DEBOUNCE_CHECKS; i++) {
		j = j & state[i];
	}
	debouncedState = j;

	if ((debouncedState == 1) && (lastDebouncedState != debouncedState)) {
		joy_stick_callback();
	} else if ((debouncedState == 2)
			&& (lastDebouncedState != debouncedState)) {
		rotary_callback();
	}
}

