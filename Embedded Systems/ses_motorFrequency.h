/*
 * ses_motorFrequency.h
 *
 *  Created on: May 28, 2019
 *      Author: Abdallah Altalmas & Shady Botros
 */

#ifndef SES_MOTORFREQUENCY_H_
#define SES_MOTORFREQUENCY_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_timer.h"
#include "ses_led.h"
#include "ses_pwm.h"
#include "ses_lcd.h"
#include "util/atomic.h"

/*
 * Defining number of intervals to be considered in the median frequency calculation
 */
#define NUMBER_OF_INTERVALS    15

/* FUNCTION PROTOTYPES *******************************************************/

void motorFrequency_init();

uint16_t motorFrequency_getRecent();

uint16_t motorFrequency_getMedian();

#endif /* SES_MOTORFREQUENCY_H_ */
