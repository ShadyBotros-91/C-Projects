/*
 * motor_controller.c
 *
 *  Created on: May 28, 2019
 *     Author: Abdallah Altalmas & Shady Botros
 */

/* INCLUDES ******************************************************************/

#include "ses_pwm.h"
#include "ses_button.h"
#include "ses_led.h"
#include "ses_scheduler.h"
#include "ses_lcd.h"
#include "ses_motorFrequency.h"

/*
 * A function indicating the number of rotary button presses
 */
bool rotaryPress() {

	if (rotaryFlag % 2 == 0) {
		return true;
	} else {
		return false;
	}
}

/*
 * A function passing a specific duty cycle to the motor
 * whenever the rotary button is pressed
 * and stopping it whenever the button is pressed again
 */
void rotaryMotorController() {

	if (rotaryPress()) {
		pwm_setDutyCycle(170);
	} else {
		pwm_setDutyCycle(255);
	}
}
/*
 * A function to be passed as a task in the scheduler for getting the
 * most recent motor's frequency in HZ and RPm and
 * the median among a previously determined number of readings in Hz and RPM
 */
void getFreq(void* ptr) {

	lcd_setCursor(0, 0);
	fprintf(lcdout, "Recent[Hz]=%d", motorFrequency_getRecent());

	lcd_setCursor(0, 1);
	fprintf(lcdout, "Recent[RPM]=%d", motorFrequency_getRecent() * 60);

	lcd_setCursor(0, 2);
	fprintf(lcdout, "Median[Hz]=%d", motorFrequency_getMedian());

	lcd_setCursor(0, 3);
	fprintf(lcdout, "Median[RPM]=%d", motorFrequency_getMedian() * 60);
}

/*
 * use the scheduler for debouncing the buttons by calling button_checkState
 *as task every 5 ms
 */

void debouncingButtons(void* ptr) {

	button_checkState();

}
/*tasks initialization*/

/*Structures for different tasks to be added to the scheduler
 * Each task shall hold { function pointer to call , pointer, which is passed to task when executed , time offset in ms, after which to call the task , period of the timer after firing}
 */

/*
 * Getting the most recent frequency every 1 second (1000ms)
 */
taskDescriptor getFrequency = { &getFreq, NULL, 1000, 1000 };

/*
 * Implementing a task to use the scheduler for debouncing the buttons by calling button_checkStateas task every 5 ms.
 */
taskDescriptor buttonsDebouncing = { &debouncingButtons, NULL, 5, 5 };

int main(void) {
	/*
	 * Initializing the LCD
	 */
	lcd_init();
	/*
	 * Initializing the LEDs
	 */
	led_redInit();
	led_greenInit();
	led_yellowInit();
	/*
	 * Initializing the button without debouncing
	 */
	button_init(false);
	/*
	 * Initializing the PWM speed control
	 */
	pwm_init();
	/*
	 * Initializing the motor frequency calculation
	 */
	motorFrequency_init();
	/*
	 * Initializing the scheduler
	 */
	scheduler_init();
	/*
	 * Task 5.1
	 * starting the motor and stopping it when the button is pressed again
	 */
	button_setRotaryButtonCallback(rotaryMotorController);

	/*
	 * Task 5.2 & 5.3 to be added to the scheduler
	 */
	scheduler_add(&getFrequency);
	/*
	 * Buttons debouncing to be added to the scheduler
	 */
	scheduler_add(&buttonsDebouncing);

	while (1) {

		/*
		 * Scheduler_ run() is run in an infinite loop
		 */
		scheduler_run();
	}

	return EXIT_SUCCESS;
}
