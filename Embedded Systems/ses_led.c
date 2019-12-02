/* INCLUDES ******************************************************************/

#include "ses_led.h"

/* DEFINES & MACROS **********************************************************/

// LED wiring on SES board
/* FUNCTION DEFINITION *******************************************************/

/*
 * Initializing red led by configuring the corresponding pin as an output and
 * then turning it off as it is active low.
 */
void led_redInit(void) {

	DDR_REGISTER(LED_RED_PORT) |= (1 << (LED_RED_PIN));
	led_redOff();

}

void led_redToggle(void) {

	LED_RED_PORT ^= (1 << (LED_RED_PIN));

}

void led_redOn(void) {

	LED_RED_PORT &= ~(1 << (LED_RED_PIN));

}

void led_redOff(void) {

	LED_RED_PORT |= (1 << (LED_RED_PIN));

}

/*
 * Initializing yellow led by configuring the corresponding pin as an output and
 * then turning it off as it is active low.
 */

void led_yellowInit(void) {
	DDR_REGISTER(LED_YELLOW_PORT) |= (1 << (LED_YELLOW_PIN));
	led_yellowOff();
}

void led_yellowToggle(void) {

	LED_YELLOW_PORT ^= (1 << (LED_YELLOW_PIN));

}

void led_yellowOn(void) {
	LED_YELLOW_PORT &= ~(1 << (LED_YELLOW_PIN));
}

void led_yellowOff(void) {
	LED_YELLOW_PORT |= (1 << (LED_YELLOW_PIN));

}

/*
 * Initializing green led by configuring the corresponding pin as an output and
 * then turning it off as it is active low.
 */
void led_greenInit(void) {
	DDR_REGISTER(LED_GREEN_PORT) |= (1 << (LED_GREEN_PIN));
	led_greenOff();

}

void led_greenToggle(void) {

	LED_GREEN_PORT ^= (1 << (LED_GREEN_PIN));

}

void led_greenOn(void) {
	LED_GREEN_PORT &= ~(1 << (LED_GREEN_PIN));
}

void led_greenOff(void) {
	LED_GREEN_PORT |= (1 << (LED_GREEN_PIN));
}

