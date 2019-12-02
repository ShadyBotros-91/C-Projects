/*
 * alarm_test.h
 *
 *  Created on: Jun 27, 2019
 *      Author: shady
 */

#ifndef ALARM_TEST_H_
#define ALARM_TEST_H_

#include "ses_common.h"
#include "ses_scheduler.h"
#include "ses_timer.h"
#include "ses_lcd.h"
#include "ses_led.h"
#include "ses_button.h"
#include "ses_rotary.h"

/* DEFINES & MACROS **********************************************************/

/*
 * Defining a transition between the states (exit conditions of the states)
 */
#define TRANSITION(newState) (fsm->state = newState, RET_TRANSITION)

#define MAX_MILLISECONDS                       999
#define MAX_SECONDS_IN_UNITS                   10
#define MAX_SECONDS                            59
#define MAX_MINUTES_IN_UNITS                   10
#define MAX_MINUTES                            59
#define MAX_HOURS_IN_UNITS                     10
#define MAX_HOURS                              23
#define TIME_PASSED_TO_STOP_ALARM              5000
#define RED_LED_4_HZ                           250

typedef uint8_t fsmReturnStatus; //< typedef to be used with return enum
typedef struct fsm_s Fsm; //< typedef for alarm clock state machine
typedef struct event_s Event; //< event type for alarm clock fsm
typedef fsmReturnStatus (*State)(Fsm *, const Event*); //typedef for state event handler functions

/* A structure to represent the finite state machine*/
struct fsm_s {
	State state; //< current state, pointer to event handler
	bool isAlarmEnabled; //< flag for the alarm status
	struct time_t timeSet; //< multi-purpose var for system time and alarm time
};

/* defining a structure for the dispatching signals (events)*/
typedef struct event_s {
	uint8_t signal; //< identifies the type of event
} Event;

/* defining a structure for the super state */
typedef struct {
	Fsm super;
} AlarmClockFsm;

/*List of possible events (Signals to be used by the alarm clock FSM) */

enum {
	ENTRY,
	JOY_STICK_PRESSED_SIG,
	ROTARY_PRESSED_SIG,
	ROTARY_ENCODER_CW,
	ROTARY_ENCODER_CCW,
	ANY_KEY_SIG,
	ALARM_MATCH,
	FIVE_SECONDS_PASSED,
	EXIT
};

/* List of possible return values from the states*/
enum {
	RET_HANDLED, //< event was handled
	RET_IGNORED, //< event was ignored; not used in this implementation
	RET_TRANSITION //< event was handled and a state transition occurred
};

/*Functions prototypes*/

/*
 * Initial state
 */
fsmReturnStatus clockInit(Fsm *fsm, const Event *e);
/*
 * Default state
 */
fsmReturnStatus clockDefault(Fsm *fsm, const Event *e);
/*
 * "Setting the hour" state
 */
fsmReturnStatus hoursSet(Fsm *fsm, const Event *e);
/*
 * "Setting the minutes" state
 */
fsmReturnStatus minutesSet(Fsm *fsm, const Event *e);
/*
 * System time is set and clock is started
 */
fsmReturnStatus clockStart(Fsm *fsm, const Event *e);
/*
 * Alarm enabled
 */
fsmReturnStatus alarmEnabled(Fsm *fsm, const Event *e);
/*
 * Alarm disabled
 */
fsmReturnStatus alarmDisabled(Fsm *fsm, const Event *e);
/*
 * Alarm ringing
 */
fsmReturnStatus alarmRings(Fsm *fsm, const Event *e);

/*
 * A function to be passed to the scheduler as a task to increment the current time every 1 millisecond
 */

void setClockTime(void*);

/*
 * A function to be passed to the scheduler as a task to check the bouncing state of the buttons
 */

void debouncingButtons(void*);

/*
 * A function to be passed to the scheduler as a task to toggle the red led at a frequency of 4 HZ when the alarm rings
 */

void redToggle(void*);

/*
 * A function to be passed to the scheduler as a task to check the bouncing state of the rotary encoder button
 */

void debouncingrotaryEncoder(void*);

/*
 * green led toggle each 1 second
 */

void greenLedToggle(void *);

#endif /* ALARM_TEST_H_ */
