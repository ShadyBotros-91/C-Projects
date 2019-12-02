/*
 * ses_rotary.h
 *
 *  Created on: Jun 26, 2019
 *      Author: shady
 */

#ifndef SES_ROTARY_H_
#define SES_ROTARY_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_timer.h"

#define A_ROTARY_PORT 	                   PORTB
#define A_ROTARY_PIN                       5
#define B_ROTARY_PORT                      PORTG
#define B_ROTARY_PIN                       2
#define ROTARY_NUM_DEBOUNCE_CHECKS         5


/* defining a function pointer for the call backs of the button */
typedef void (*pTypeRotaryCallback)();

/* Initializing the button with the ability to check for the bouncing condition*/
void rotary_init(bool debouncing);

/* Checking whether the button is rotated CW*/
bool rotary_isClockwiseRotated();

/* Checking whether the button is rotated CCW*/
bool rotary_isCounterClockwiseRotated();

/* setting the call back function for the CW direction */
void rotary_setClockwiseCallback(pTypeRotaryCallback);

/* setting the call back function for the CCW direction */
void rotary_setCounterClockwiseCallback(pTypeRotaryCallback);

/* A function used to check the bouncing state of the button*/
void rotary_checkState();

#endif /* SES_ROTARY_H_ */
