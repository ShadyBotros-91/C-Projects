#ifndef SES_LED_H_
#define SES_LED_H_

#include "ses_common.h"

#define LED_RED_PORT       	  PORTG
#define LED_RED_PIN         	1

#define LED_YELLOW_PORT 	  PORTF
#define LED_YELLOW_PIN      	7

#define LED_GREEN_PORT 		  PORTF
#define LED_GREEN_PIN       	6


/* FUNCTION PROTOTYPES *******************************************************/

/**initializes red led
 */
void led_redInit(void);

/**toggles red led
 */
void led_redToggle(void);

/**enabled red led
 */
void led_redOn(void);

/**disables red led
 */
void led_redOff(void);

/**initializes yellow led
 */
void led_yellowInit(void);

/**toggles yellow led
 */
void led_yellowToggle(void);

/**enabled yellow led
 */
void led_yellowOn(void);

/**disables yellow led
 */
void led_yellowOff(void);
/**initializes green led
 */
void led_greenInit(void);

/**toggles green led
 */
void led_greenToggle(void);

/**enabled green led
 */
void led_greenOn(void);

/**disables green led
 */
void led_greenOff(void);

#endif /* SES_LED_H_ */
