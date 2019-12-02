/*
 * ses_adc.c
 *
 *  Created on: Apr 21, 2019
 *      Author: shady
 */
#include "ses_adc.h"
#include "ses_lcd.h"
#include "ses_button.h"

/* DEFINES & MACROS **********************************************************/

// Button wiring on SES board
/* Initializing ADC
 */

void adc_init(void) {
	/*Configure the data direction registers (temperature, light, microphone, joystick) and deactivate
	 their internal pull-up resistors*/
	DDR_REGISTER(ADC_PORT) &= ~((1 << JOYSTICK_PIN) | (1 << LIGHT_SENSOR_PIN)
			| (1 << TEMP_SENSOR_PIN));
	ADC_PORT &= ~((1 << JOYSTICK_PIN) | (1 << LIGHT_SENSOR_PIN)
			| (1 << TEMP_SENSOR_PIN));

	//Disable power reduction mode for the ADC module

	PRR0 &= ~(1 << PRADC);

	//Using ADC_VREF_SRC macro to configure the voltage reference in register ADMUX.
	//Configuring the ADLAR bit, which should set the ADC result right adjusted.
	/*ADMUX &= ~(0xC0);*/
	/*ADMUX &= ~(0x20);*/
	ADMUX |= ((1 << REFS0) | (1 << REFS1));
	ADMUX |= 1 << ADLAR;

	//Configuring the prescaler in register ADCSRA.
	//Enabling the ADC (ADEN)

	ADCSRA |= (ADC_PRESCALE | (1 << ADEN)); //011 or 111-->fastest possible

	//Do not select auto triggering (ADATE)
	ADCSRA &= ~(1 << ADATE);

}

uint16_t adc_read(uint8_t adc_channel) {

	//CLearing ADMUX register at the beginning
	ADMUX &= 0xf0;

	if ((adc_channel > ADC_NUM) | (adc_channel < 0)) {

		return ADC_INVALID_CHANNEL;
	} else {

		ADMUX |= adc_channel;
//Starting conversion
		//reading from ADCL and ADCH ???

		ADCSRA |= (1 << ADSC);
		while ((ADCSRA & (1 << ADSC)))
			;
		ADCSRA &= ~(1 << ADSC);

		return ADC;
	}
}

uint8_t adc_getJoystickDirection() {

	uint16_t direction = adc_read(ADC_JOYSTICK_CH);
	uint8_t stick_direction = 0;
	if ((direction >= LOW_ERROR_RIGHT) && (direction <= HIGH_ERROR_RIGHT)) {
		fprintf(lcdout, "Right");
		stick_direction = RIGHT;
	} else if ((direction >= LOW_ERROR_UP) && (direction <= HIGH_ERROR_UP)) {
		fprintf(lcdout, "Up");
		stick_direction = UP;
	} else if ((direction >= LOW_ERROR_LEFT)
			&& (direction <= HIGH_ERROR_LEFT)) {
		fprintf(lcdout, "Left");
		stick_direction = LEFT;
	} else if ((direction >= LOW_ERROR_DOWN)
			&& (direction <= HIGH_ERROR_DOWN)) {
		fprintf(lcdout, "Down");
		stick_direction = DOWN;
	} else {
		fprintf(lcdout, "No direction");
		stick_direction = NO_DIRECTION;
	}
	return stick_direction;
}

int16_t adc_getTemperature() {

	int16_t adc = (int16_t)adc_read(ADC_TEMP_CH);

 //using 40 and 20 for interpolation
	int16_t slope = (ADC_TEMP_MAX - ADC_TEMP_MIN)
			/ ((ADC_TEMP_RAW_MAX - ADC_TEMP_RAW_MIN));
	int16_t offset = ADC_TEMP_MAX - (ADC_TEMP_RAW_MAX * slope);
	return (adc * slope + offset) / ADC_TEMP_FACTOR;
}
