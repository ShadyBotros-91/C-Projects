/*
 * ses_motorFrequency.c
 *
 *  Created on: May 28, 2019
 *     Author: Abdallah Altalmas & Shady Botros
 */

/*Includes*************************************************************************************/

#include "ses_motorFrequency.h"

//Private variables

/*
 * Number of current drops (current spikes)
 */
volatile static uint16_t counter = 0;
/*
 * Number of revolutions of the motor
 */
volatile static uint16_t rev = 0;
/*
 * Time of one motor's revolution
 */
volatile static uint16_t oneRevTime = 0;
/*
 * An array to save the time measurements for the the motor's revolutions
 */
volatile static uint16_t measurements[NUMBER_OF_INTERVALS] = { };
/*
 * A boolean to show the state of the motor
 * at the beginning , the motor is stopped
 */
volatile bool stoppedMotor = true;
/*
 * The most recent motor's frequency
 */
static uint16_t recentFrequency = 0;
/*
 * The median frequency among a previously defined number of measurements
 */
static uint16_t medianFrequency = 0;

/* FUNCTIONs IMPLEMENTATION *******************************************************/

/*
 * int compare(const void*a, const void* b)
 *
 * @param const void* --->void pointer which can be cast into any type (elements of the array)
 *
 * @return int---> compare returns int
 *
 *--->compare is a function needed or the quick sort function .
 *it takes two elements of the array , comparing them and returns positive value
 *if the first element is greater than the second one and returns a negative value otherwise
 */
static int compare(const void*a, const void* b) {

	int A = *((int*) a);
	int B = *((int*) b);
	return A - B;
}

/*
 * The interrupt service routine for the external interrupt (INT0)
 * the number of current drops is being counted , when it reaches 6 (meaning a complete motor revolution) ,
 * the time for this revolution is calculated and saved in the array and then the number of revolutions is incremented
 * and the timer counter is reset for the next time calculation
 */
ISR(INT0_vect) {

	led_yellowToggle();
	rev = 0;
	counter++;
	if (counter % 6 == 0) {
		counter = 0;
		oneRevTime = (TCNT5 * 100);
		oneRevTime = oneRevTime / TIMER5_CYC_FOR_100MILLISEC;
		measurements[rev] = oneRevTime;
		rev++;
		TCNT5 = 0;
	}
	/*
	 * Making sure the readings are saved at the beginning of the array every time the array is full
	 */
	if (rev == NUMBER_OF_INTERVALS) {
		rev = 0;
	}
}
/*
 * The interrupt service routine of Timer5
 * used for indicating a stopped motor every 100ms by lighting up the green led
 */
ISR(TIMER5_COMPA_vect) {

	if (counter == 0) {
		stoppedMotor = true;
		led_greenOn();
	} else {
		stoppedMotor = false;
		led_greenOff();
	}
}

/*
 * Initializing the motor frequency calculation by starting timer5 and configuring external interrupt (INT0)
 */
void motorFrequency_init() {

	/*
	 * starting timer5
	 */

	timer5_start();

	//Configuring INT0
	/*
	 * Therefore, it is recommended to first disable INTn by clearing its Interrupt Enable bit in the EIMSK Register.
	 * Then, the ISCn bit can be changed (Configuring External Interrupt 0 Sense Control Bit for the rising edge).
	 * Finally, the INTn interrupt flag should be cleared by writing a logical one to its Interrupt Flag bit (INTFn)
	 * in the EIFR Register before the interrupt is re-enabled.
	 *
	 */
	EIMSK &= ~(1 << INT0);
	EICRA |= ((1 << ISC01) | (1 << ISC00));
	EIFR |= (1 << INTF0);
	EIMSK |= (1 << INT0);

}

/*
 * A function returning the most recent reading for the motor's frequency in HZ
 */
uint16_t motorFrequency_getRecent() {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (stoppedMotor) {
			recentFrequency = 0;
		} else {
			recentFrequency = 1000 / oneRevTime;
		}
	}
	return recentFrequency;
}
/*
 * A function returning the median motor's frequency among a previously defined number of readings in HZ
 */
uint16_t motorFrequency_getMedian() {

	/*
	 * A copy of the array of time measurements
	 */
	static uint16_t measurementsCopy[NUMBER_OF_INTERVALS] = { };
	static uint8_t i = 0;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		for (i = 0; i < NUMBER_OF_INTERVALS; i++) {
			measurementsCopy[i] = measurements[i];
		}
	}
	/*
	 * Sorting the copied array in an ascending order using the quick sort function
	 */
	qsort(measurementsCopy, NUMBER_OF_INTERVALS, sizeof(uint16_t), compare);

	if (stoppedMotor) {
		return 0;
	}
	/*
	 * If the number of defined intervals is even
	 */
	else if (NUMBER_OF_INTERVALS % 2 == 0) {
		medianFrequency = (1000
				/ (measurementsCopy[NUMBER_OF_INTERVALS / 2]
						+ measurementsCopy[(NUMBER_OF_INTERVALS + 1) / 2] / 2));
		return medianFrequency;
	}
	/*
	 * If the number of intervals is odd
	 */
	else {
		medianFrequency = (1000 / (measurementsCopy[NUMBER_OF_INTERVALS / 2]));
		return medianFrequency;
	}

}
