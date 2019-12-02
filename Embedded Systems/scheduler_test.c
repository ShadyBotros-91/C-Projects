/*
 * scheduler_test.c
 *
 *  Created on: May 14, 2019
 *      Author: Abdallah ALTalmas & Shady Botros
 */

/* INCLUDES ******************************************************************/

#include "ses_timer.h"
#include "ses_led.h"
#include "ses_button.h"
#include "ses_scheduler.h"
#include "ses_lcd.h"

/*Function prototypes*/

void selectLed(void *ptr);
void led_yellowOffparam(void *ptr);
void debouncingButtons(void* ptr);
void joyStickLedToggle();
void stopWatch(void* ptr);
void rotaryStopWatch();

/*
 * enumerator implemented to choose among different led colors
 */
enum led {
	red, green, yellow
} color;

/*
 * Stop Watch parameters
 */
static uint8_t counter = 0;
static uint8_t seconds = 0;
static uint8_t tenth = 0;

/*
 *Flag for Rotary button press
 */
static uint8_t rotaryPressed = 1;

/*
 * Flag for Joy Stick button press
 */
static uint8_t joyPressed = 1;

/*
 * Led color selection
 */
enum led color = green;

/*tasks initialization*/

/*Structures for different tasks to be added to the scheduler
 * Each task shall hold { function pointer to call , pointer, which is passed to task when executed , time offset in ms, after which to call the task , period of the timer after firing}
 */

/*toggling the green LED with a frequency of 0.5 1/s
 */
taskDescriptor task_1 = { &selectLed, &color, 500, 500 };

/*
 * Implementing a task to use the scheduler for debouncing the buttons by calling button_checkStateas task every 5 ms.
 */
taskDescriptor task_2 = { &debouncingButtons, NULL, 5, 5 };

/*
 * Implementing a task to switch off the yellow LED if it is still on without pushing the joy stick button
 */
taskDescriptor task_3 = { &led_yellowOffparam, NULL, 5000, 5000 };

/*
 * Implement a stop watch which starts and stops when pressing the rotary button.
 */
taskDescriptor task_4 = { &stopWatch, NULL, 1, 1 };

/*
 * Implementing a task to toggle an LED and use the parameter to select the color, e.g. by the enum.
 * It includes casting the passed parameter into a pointer to an integer and then dereferencing the value to obtain the color
 */

void selectLed(void *ptr) {

	uint8_t* parameter = (uint8_t*) ptr;

	switch (*parameter) {
	case red:
		led_redToggle();
		break;

	case green:
		led_greenToggle();
		break;

	case yellow:
		led_yellowToggle();
		break;

	default:
		break;
	}

}

void led_yellowOffparam(void *ptr) {

	led_yellowOff();
}

/*use the scheduler for debouncing the buttons by calling button_checkState
 *as task every 5 ms
 */

void debouncingButtons(void* ptr) {

	button_checkState();

}

/*
 * A call back function for the joy stick button
 * When pressing the joystick button, toggle the yellow LED and add the assigned task to the scheduler.
 *if the button is pressed again ,the led turns off and  the task is removed from the scheduler
 */

void joyStickLedToggle() {

	if (joyPressed == 1) {
		joyPressed = 0;
		led_yellowToggle();
		scheduler_add(&task_3);

	} else {
		joyPressed = 1;
		led_yellowOff();

		scheduler_remove(&task_3);
		task_3.expire = 5000;

	}
}

/*
 * Implement a stop watch which starts and stops when pressing the rotary button.
 * the counter increases till it reaches 100 ms , then the tenth of a second is incremented and if it reaches 10 ms (100*10=1000ms= 1 sec)
 * the number of seconds is incremented and the counter starts from zero.
 * The stop watch should start all over again after 59 seconds
 */

void stopWatch(void* ptr) {

	counter++;
	if (counter == 100) { // ten
		tenth++;
		counter = 0;
		if (tenth == 10) {
			seconds++;
			tenth = 0;
			counter = 0;
		}
	}
	if (seconds == 60) {
		seconds = 0;
		counter = 0;
	}

	lcd_setCursor(0, 0);
	fprintf(lcdout, " %d : %d \n", seconds, tenth);

}

/*A call back function for the rotary button
 * The rotary flag is incremented in the ISR of the rotary button , and then the assigned task is added to the scheduler.
 * If the rotary button is pressed again , the flag is incremented once again and the task is removed from the scheduler
 *
 */
void rotaryStopWatch() {
	if (rotaryFlag % 2 != 0 && rotaryPressed == 0) {
		rotaryPressed = 1;
		scheduler_remove(&task_4);
		lcd_clear();
		lcd_setCursor(0, 0);
		fprintf(lcdout, " %d : %d \n", seconds, tenth);

	} else if (rotaryFlag % 2 == 0 && rotaryPressed == 1) {
		rotaryPressed = 0;
		scheduler_add(&task_4);
	}
}

int main(void) {

	/*
	 * Button initialization without including timer 1 for the debouncing
	 */
	button_init(false);

	/*
	 * Leds initialization
	 */
	led_redInit();
	led_greenInit();
	led_yellowInit();

	/*
	 * LCD initialization
	 */
	lcd_init();

	/*
	 * Scheduler initialization
	 */
	scheduler_init();

	/*
	 * Joy stick button call back
	 */
	button_setJoystickButtonCallback(&joyStickLedToggle);

	/*
	 * Rotary button call back
	 */
	button_setRotaryButtonCallback(&rotaryStopWatch);

	/*
	 * Adding task 1 and 2 to the scheduler
	 *
	 */
	scheduler_add(&task_1);
	scheduler_add(&task_2);

	while (1) {
		/*
		 * Scheduler_ run is run in an infinite loop
		 */
		scheduler_run();
	}
	return EXIT_SUCCESS;

}

