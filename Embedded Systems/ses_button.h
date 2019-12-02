#ifndef SES_BUTTON_H_
#define SES_BUTTON_H_

/* INCLUDES ******************************************************************/

#include "ses_common.h"
#include "ses_timer.h"

#define BUTTON_PORT 	             PORTB
#define BUTTON_ROTARY_ENCODER_PIN      6
#define BUTTON_JOYSTICK_PIN       	   7
#define BUTTON_NUM_DEBOUNCE_CHECKS     5

/*A Flag which is incremented whenever , the rotary button is pressed
 *
 */

/* FUNCTION PROTOTYPES *******************************************************/

/**
 * Initializes rotary encoder and joystick button
 */
void button_init(bool debouncing);

/** 
 * Get the state of the joystick button.
 */
bool button_isJoystickPressed(void);

/** 
 * Get the state of the rotary button.
 */
bool button_isRotaryPressed(void);

typedef void (*pButtonCallback)();

void button_setRotaryButtonCallback(pButtonCallback callback);

void button_setJoystickButtonCallback(pButtonCallback callback);

void button_checkState();

#endif /* SES_BUTTON_H_ */
