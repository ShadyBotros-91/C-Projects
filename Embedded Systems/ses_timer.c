/* INCLUDES ******************************************************************/
#include "ses_timer.h"

//call-back functions for each timer
volatile pTimerCallback tcb1 = NULL;
volatile pTimerCallback tcb2 = NULL;
volatile pTimerCallback tcb5 = NULL;

/*FUNCTION DEFINITION ********************************************************/

void timer2_setCallback(pTimerCallback cb) {

	if (cb != NULL)
		tcb2 = cb;

}
//Use timer2
void timer2_start() {

	/*
	 * Globally enable all interrupts
	 */
	sei();

	/*
	 *Power Reduction Timer/Counter2 bit PRTIM2 in Power Reduction Register must be written to zero to enable Timer/Counter2 module.
	 */
	PRR0 &= ~(1 << PRTIM2);

	//Use Clear Timer on Compare Match (CTC) mode operation

	/*
	 *Waveform Generation Mode Bits in Timer/Counter2 Control Register A should be manipulated
	 */

	TCCR2A &= ~(1 << WGM20);
	TCCR2A |= 1 << WGM21;
	TCCR2B &= ~(1 << WGM22);

	//Select a pre-scaler of 64

	/*
	 * Clock select bits in Timer/Counter2 Control Register B should be manipulated
	 */

	TCCR2B &= ~((1 << CS20) | (1 << CS21));
	TCCR2B |= 1 << CS22;

	//Set interrupt mask register for Compare A

	/*
	 * Timer/Counter2 Output Compare Match A Interrupt Enable bit is set in the Timer/Counter Interrupt Mask register
	 */

	TIMSK2 |= (1 << OCIE2A);

	//Set a value in register OCR2A in order to generate an interrupt every 1 ms

	/*
	 *Timer/Counter2 Output Compare Register A
	 */
	OCR2A = TIMER2_CYC_FOR_1MILLISEC;

	//Clear the interrupt flag by setting an 1 in flag register for Compare A

	/*
	 * Output Compare Flag 2 A bit is cleared by writing a logic one in the Timer/Counter Interrupt Flag Register
	 */

	TIFR2 |= (1 << OCF2A);
}

void timer2_stop() {

	//clear clock source

	TCCR2B &= ~((1 << CS20) | (1 << CS21) | (1 << CS22));

	//Disabling power reduction

	PRR0 |= (1 << PRTIM2);

}

void timer1_setCallback(pTimerCallback cb) {

	if (cb != NULL)
		tcb1 = cb;
}

//use timer 1
void timer1_start() {
	/*
	 * Globally enable all interrupts
	 */
	sei();

	/*
	 *Power Reduction Timer/Counter1 bit PRTIM1 in Power Reduction Register must be written to zero to enable Timer/Counter1 module.
	 */
	PRR0 &= ~(1 << PRTIM1);

	//Use Clear Timer on Compare Match (CTC) mode operation

	/*
	 *Waveform Generation Mode Bits in Timer/Counter1 Control Register A & B should be manipulated
	 */
	TCCR1A &= ~(1 << WGM10);
	TCCR1A &= ~(1 << WGM11);
	TCCR1B |= (1 << WGM12);
	TCCR1B &= ~(1 << WGM13);

	//Select a pre-scaler of 64

	/*
	 * Clock select bits in Timer/Counter1 Control Register B should be manipulated
	 */

	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS11);
	TCCR1B &= ~(1 << CS12);

	//Set a value in register OCR1A in order to generate an interrupt every 5 ms

	/*
	 *Timer/Counter1 Output Compare Register A
	 */

	OCR1A = TIMER1_CYC_FOR_5MILLISEC;

	//Set interrupt mask register for Compare A

	/*
	 * Timer/Counter1 Output Compare Match A Interrupt Enable bit is set in the Timer/Counter Interrupt Mask register
	 */
	TIMSK1 |= (1 << OCIE1A);

	//Clear the interrupt flag by setting an 1 in flag register for Compare A

	/*
	 * Output Compare Flag 1 A bit is cleared by writing a logic one in the Timer/Counter Interrupt Flag Register
	 */
	TIFR1 |= (1 << OCF1A);

}

void timer1_stop() {

	//clear clock source

	TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));

	//Disabling power reduction

	PRR0 |= (1 << PRTIM1);

}

void timer5_start() {

	/*
	 *Power Reduction Timer/Counter5 bit PRTIM5 in Power Reduction Register PRR1 must be written to zero to enable Timer/Counter5 module.
	 */
	PRR1 &= ~(1 << PRTIM5);

	/*
	 * Globally enable all interrupts
	 */
	sei();

	/*
	 *Using Clear Timer on Compare Match (CTC) mode operation
	 *Waveform Generation Mode Bits in Timer/Counter5 Control Register A & B should be manipulated
	 */

	TCCR5A &= ~((1 << WGM50) | (1 << WGM51));
	TCCR5B |= (1 << WGM52);
	TCCR5B &= ~(1 << WGM53);

	/*
	 * Selecting a pre-scaler of 64S
	 * Clock select bits in Timer/Counter1 Contro5 Register B should be manipulated
	 */

	TCCR5B |= ((1 << CS50) | (1 << CS51));
	TCCR5B &= ~(1 << CS52);

	//Set a value in register OCR5A in order to generate an interrupt every 100 ms

	/*
	 *Timer/Counter5 Output Compare Register A
	 */

	OCR5A = TIMER5_CYC_FOR_100MILLISEC;

	//Set interrupt mask register for Compare A

	/*
	 * Timer/Counter5 Output Compare Match A Interrupt Enable bit is set in the Timer/Counter Interrupt Mask register
	 */
	TIMSK5 |= (1 << OCIE5A);

	//Clear the interrupt flag by setting an 1 in flag register for Compare A

	/*
	 * Output Compare Flag 5A bit is cleared by writing a logic one in the Timer/Counter Interrupt Flag Register
	 */
	TIFR5 |= (1 << OCF5A);

}

void timer5_setCallback(pTimerCallback cb) {

	if (cb != NULL)
		tcb5 = cb;

}

void timer5_stop() {

	//clear clock source

	TCCR5B &= ~((1 << CS50) | (1 << CS51) | (1 << CS52));

	//Disabling power reduction

	PRR1 |= (1 << PRTIM5);

}

void timer0_start() {

	/*
	 *Power Reduction Timer/Counter0 bit PRTIM0 in Power Reduction Register must be written to zero to enable Timer/Counter0 module.
	 */
	PRR0 &= ~(1 << PRTIM0);

	/*
	 * Configure the timer registers to set the OC0B pin when the counter reaches the value of OCR0B.(inverting mode)
	 * and choosing the top value of the fast PWM to be 0XFF.
	 */

	TCCR0A |= ((1 << COM0B1) | (1 << COM0B0) | (1 << WGM01) | (1 << WGM00));
	TCCR0B &= ~(1 << WGM02);

	/*
	 * Disabling the pre-scaler
	 */

	TCCR0B &= ~((1 << CS02) | (1 << CS01));
	TCCR0B |= (1 << CS00);

	/*
	 * this bit must be set to zero when TCCR0B is written in a PWM operation mode.
	 */

	TCCR0B &= ~((1 << FOC0A) | (1 << FOC0B));

	/*
	 * Configuring the starting point of OCR0B to be 255 , thus reading a "0" means full duty cycle
	 */
	OCR0B = 0xff;

	/*
	 * Enabling Global interrupt bit
	 */
	sei();
}

void timer0_stop() {
	/*
	 * clear clock source
	 */
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
	/*
	 * Disabling power reduction
	 */
	PRR0 |= (1 << PRTIM0);
}
//Interrupt service routine for timer 1

ISR(TIMER1_COMPA_vect) {

	tcb1();

}

//Interrupt service routine for timer 2

ISR(TIMER2_COMPA_vect) {

	tcb2();
}
