#ifndef SES_TIMER_H_
#define SES_TIMER_H_

/*INCLUDES *******************************************************************/

#include "ses_common.h"
#include "ses_lcd.h"
#include "ses_motorFrequency.h"
#include "ses_scheduler.h"

/* DEFINES & MACROS **********************************************************/

// F_CPU / ((required_freq*pre_scaler)-1)
#define TIMER1_CYC_FOR_5MILLISEC       1245

#define TIMER2_CYC_FOR_1MILLISEC       249

#define TIMER5_CYC_FOR_100MILLISEC     6249

/*PROTOTYPES *****************************************************************/

/**type of function pointer used as timer callback
 */
typedef void (*pTimerCallback)(void);

/*
 * Sets a function to be called when the timer fires. If NULL is
 * passed, no callback is executed when the timer fires.
 *
 * @param cb  valid pointer to callback function
 */
void timer2_setCallback(pTimerCallback cb);

/*
 * Starts hardware timer 2 of MCU with a period
 * of 1 ms.
 */
void timer2_start();

/*
 * Stops timer 2.
 */
void timer2_stop();

/*
 * Sets a function to be called when the timer fires.
 *
 * @param cb  pointer to the callback function; if NULL, no callback
 *            will be executed.
 */
void timer1_setCallback(pTimerCallback cb);

/*
 * Start timer 1 of MCU to trigger on compare match every 5ms.
 */
void timer1_start();

/*
 * Stops timer 1 of the MCU if it is no longer needed.
 */
void timer1_stop();

/*
 * Sets a function to be called when the timer fires.
 *
 * @param cb  pointer to the callback function; if NULL, no callback
 *            will be executed.
 */
void timer5_setCalback(pTimerCallback cb);
/**
 * Start timer 5 of MCU to trigger on compare match every 1ms.
 */
void timer5_start();

/*
 * Stops timer 5 of the MCU if it is no longer needed.
 */
void timer5_stop();
/*
 * Start timer 0 of MCU in a fast PWM generation mode
 */

void timer0_start();

/*
 * Stops timer 5 of the MCU if it is no longer needed.
 */

void timer0_stop();

#endif /* SES_TIMER_H_ */
