/*Includes ***************************************************************/

#include "ses_pwm.h"
#include "ses_timer.h"

/* FUNCTION IMPLEMENTATION *******************************************************/

void pwm_init(void) {

	/*
	 * Configuring OC0B pin (connected to PORTG5) as output
	 */
	DDR_REGISTER(TRANSISTOR_PORT) |= (1 << TRANSISTOR_PIN);
	/*
	 * Starting Timer0
	 */
	timer0_start();

}

void pwm_setDutyCycle(uint8_t dutyCycle) {

	/*
	 * Passing the duty cycle to the output compare register B (to generate PWM on 0C0B pin)
	 */
	OCR0B = dutyCycle;

}
