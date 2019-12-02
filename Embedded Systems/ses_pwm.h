#ifndef SES_PWM_H_
#define SES_PWM_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_timer.h"

/*
 * Configuring OC0B pin
 */
#define TRANSISTOR_PORT 	             PORTG
#define TRANSISTOR_PIN                     5

/* FUNCTION PROTOTYPES *******************************************************/

/*
 * Initializing the PWM speed control
 */
void pwm_init(void);

/*
 * Adjusting the duty cycle to control the motor speed
 */

void pwm_setDutyCycle(uint8_t dutyCycle);

#endif /* SES_PWM_H_ */
