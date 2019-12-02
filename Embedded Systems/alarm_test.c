/*
 * alarm_clock.c
 *
 *  Created on: Jun 20, 2019
 *      Author: shady
 */
#include "alarm_test.h"

/** Task Descriptors for dispatching tasks into the scheduler */

/* Task for checking the bouncing state of the buttons every 5 milliseconds*/

taskDescriptor buttonsDebouncing = { &debouncingButtons, NULL, 5, 5 };

/* Task for checking the bouncing state of the rotary encoder button every 5 milliseconds*/

taskDescriptor encoderDebouncing = { &debouncingrotaryEncoder, NULL, 5, 5 };

/* Task for displaying the current time (scheduled every 1 millisecond) */

taskDescriptor displayTime = { &setClockTime, NULL, 1, 1 };

/* Task for triggering the alarm time set by the user and toggling the red led  */

taskDescriptor triggerAlarm = { &redToggle, NULL, 1, 1 };

/*Task for toggling the green led every 1 second*/

taskDescriptor greenToggle = { &greenLedToggle, NULL, 1000, 1000 };

/* Private variables */

/* An instance of the alarm clock finite state machine structure representing the super state  */

AlarmClockFsm alarmClock;

/*FUNCTIONS DEFINITION *************************************************/

/*Static functions*/

/* Main function responsible for dispatching events to the finite state machine of the alarm clock */
inline static void fsm_dispatch(Fsm* fsm, const Event* event) {

	/* a signal representing the entry point to a state*/
	static Event entryEvent = { .signal = ENTRY };
	/* a signal representing the exit point from the state*/
	static Event exitEvent = { .signal = EXIT };
	/* Executing the current state*/
	State s = fsm->state;
	/* checking the return status of the different cases inside the state */
	fsmReturnStatus r = fsm->state(fsm, event);
	/* if transition is returned --> the next state is called*/
	if (r == RET_TRANSITION) {
		s(fsm, &exitEvent); //< call exit action of last state
		fsm->state(fsm, &entryEvent); //< call entry action of new state
	}
}

/* Entry state of the alarm clock FSM */
inline static void fsm_init(Fsm* fsm, State init) {
	/* a signal representing the entry point to initial state*/
	Event entryEvent = { .signal = ENTRY };
	/* Executing the initial state */
	fsm->state = init;
	/* Dispatching the entry signal to the FSM*/
	fsm->state(fsm, &entryEvent);
}

/* Call back function for the joy stick button --> Actual signal for the enumerated one*/

static void joystickPressedDispatch() {
	Event e = { .signal = JOY_STICK_PRESSED_SIG };
	fsm_dispatch((Fsm*) &alarmClock, &e);
}

/* Call back function for the rotary button --> Actual signal for the enumerated one*/

static void rotaryPressedDispatch() {
	Event e = { .signal = ROTARY_PRESSED_SIG };
	fsm_dispatch((Fsm*) &alarmClock, &e);
}

/* Call back function for the rotary encoder (clock wise direction) --> Actual signal for the enumerated one*/

static void rotaryEncoderClockwiseDispatch() {
	Event e = { .signal = ROTARY_ENCODER_CW };
	fsm_dispatch((Fsm*) &alarmClock, &e);
}

/* Call back function for the rotary encoder (counter clock wise direction) --> Actual signal for the enumerated one*/

static void rotaryEncoderCounterclockwiseDispatch() {
	Event e = { .signal = ROTARY_ENCODER_CCW };
	fsm_dispatch((Fsm*) &alarmClock, &e);
}

/*Global functions*/

/* This function increments the clock every millisecond and display it every second to the user on the LCD */

void setClockTime(void *ptr) {
	/* incrementing the time in milliseconds*/
	currentTime.milli++;
	if (currentTime.milli > MAX_MILLISECONDS) {

		/* incrementing the time in seconds and toggling the green led*/
		currentTime.milli = 0;
		currentTime.second++;
		if (currentTime.second > MAX_SECONDS) {
			currentTime.second = 0;
			currentTime.minute++;
		}
		if (currentTime.second < MAX_SECONDS_IN_UNITS) {
			lcd_setCursor(13, 0);
			fprintf(lcdout, "0%u", currentTime.second);
		}
		if (currentTime.second >= MAX_SECONDS_IN_UNITS) {
			lcd_setCursor(13, 0);
			fprintf(lcdout, "%u", currentTime.second);
		}
		/* incrementing the time in minutes */
		if (currentTime.minute < MAX_MINUTES_IN_UNITS) {
			lcd_setCursor(8, 0);
			fprintf(lcdout, "0%u", currentTime.minute);
		}
		if (currentTime.minute >= MAX_MINUTES_IN_UNITS) {
			lcd_setCursor(8, 0);
			fprintf(lcdout, "%u", currentTime.minute);
		}
		if (currentTime.minute > MAX_MINUTES) {
			/* incrementing the time in hours*/
			currentTime.minute = 0;
			currentTime.hour++;
			if (currentTime.hour > MAX_HOURS) {
				currentTime.hour = 0;
			}
			if (currentTime.hour < MAX_HOURS_IN_UNITS) {
				lcd_setCursor(3, 0);
				fprintf(lcdout, "0%u", currentTime.hour);
			} else {
				lcd_setCursor(3, 0);
				fprintf(lcdout, "%u", currentTime.hour);
			}
		}
	}

}

void debouncingButtons(void* ptr) {

	button_checkState();
}

void debouncingrotaryEncoder(void * ptr) {

	rotary_checkState();
}

/* This function triggers the alarm set by the user and toggles the red led at a frequency of 4 HZ
 *  and also sets a counter to stop the alarm after 5 seconds and dispatch this signal to the FSM  */
void redToggle(void *ptr) {

	/* checking that the current time reaches the alarm in hours and in minutes*/
	if (currentTime.hour == alarmTime.hour
			&& currentTime.minute == alarmTime.minute) {
		/* a static counter to measure 5 seconds to stop the alarm */

		static uint16_t fiveSecCounter = 0;

		/* a static counter to adjust the frequency of the red led toggling*/
		static uint16_t redLedCounter = 0;
		fiveSecCounter++;
		redLedCounter++;
		if (fiveSecCounter == TIME_PASSED_TO_STOP_ALARM) {
			/*After 5 seconds passed the counters are reset for the next alarm setting*/
			fiveSecCounter = 0;
			redLedCounter = 0;
			/* dispatching this signal to the FSM*/
			Event e = { .signal = FIVE_SECONDS_PASSED };
			fsm_dispatch((Fsm*) &alarmClock, &e);
			return;
		}
		if (redLedCounter == RED_LED_4_HZ) {
			led_redToggle();
			redLedCounter = 0;
		}
	}
}

void greenLedToggle(void *ptr) {
	led_greenToggle();
}
/*States' Functions definition*/

/* Initial state (State 0) */
fsmReturnStatus clockInit(Fsm *fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		/* Asking the user to set the time*/
		lcd_setCursor(6, 0);
		fprintf(lcdout, "HH : MM \n");
		fprintf(lcdout, "Please press RB to \n ");
		fprintf(lcdout, "set the time ");
		/* Making sure the bouncing states of the buttons are checked by running these tasks in the scheduler*/
		scheduler_add(&buttonsDebouncing);
		scheduler_add(&encoderDebouncing);
		/* moving to the next state*/
		return TRANSITION(clockDefault);
	default:
		return RET_IGNORED;
	}
}

/* State for the user to set the time ( State 1)*/
fsmReturnStatus clockDefault(Fsm *fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		return RET_HANDLED;
		/*First rotary button press required to enter the setting mode*/
	case ROTARY_PRESSED_SIG:
		/*starting the time settings*/
		lcd_clear();
		lcd_setCursor(6, 0);
		fprintf(lcdout, "00 : 00");
		/* moving to the next state*/
		return TRANSITION(hoursSet);
	default:
		return RET_IGNORED;
	}
}
/* State for setting the hours ( State 2)*/
fsmReturnStatus hoursSet(Fsm *fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		return RET_HANDLED;
		/* In case the rotary encoder button is rotated CW*/
	case ROTARY_ENCODER_CW:
		/* whenever the previous state is alarmEnabled*/
		if (fsm->isAlarmEnabled) {
			/*incrementing the alarm hours and adjusting the display and overflow of the readings*/
			if (alarmTime.hour >= MAX_HOURS) {
				alarmTime.hour = 0;
				lcd_setCursor(6, 0);
				fprintf(lcdout, "0%u", alarmTime.hour);
			} else {
				alarmTime.hour++;
				if (alarmTime.hour < MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "0%u", alarmTime.hour);
				} else if (alarmTime.hour >= MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "%u", alarmTime.hour);
				}
			}
		}

		/* whenever the previous state is the default*/
		else {
			/*incrementing the current hours and adjusting the display and overflow of the readings*/
			if (currentTime.hour >= MAX_HOURS) {
				currentTime.hour = 0;
				lcd_setCursor(6, 0);
				fprintf(lcdout, "0%u", currentTime.hour);
			} else {
				currentTime.hour++;
				if (currentTime.hour < MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "0%u", currentTime.hour);
				} else if (currentTime.hour >= MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "%u", currentTime.hour);
				}
			}

		}
		return RET_HANDLED;
		/* In case the rotary encoder button is rotated CCW*/
	case ROTARY_ENCODER_CCW:
		/* whenever the previous state is alarmEnabled*/
		if (fsm->isAlarmEnabled) {
			/*decrementing the alarm hours and adjusting the display and overflow of the readings*/
			if (alarmTime.hour <= 0) {
				alarmTime.hour = MAX_HOURS;
				lcd_setCursor(6, 0);
				fprintf(lcdout, "%u", alarmTime.hour);
			} else {
				alarmTime.hour--;
				if (alarmTime.hour < MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "0%u", alarmTime.hour);
				} else if (alarmTime.hour >= MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "%u", alarmTime.hour);
				}
			}
		}

		/* whenever the previous state is the default*/
		else {
			/*decrementing the current hours and adjusting the display and overflow of the readings*/
			if (currentTime.hour <= 0) {
				currentTime.hour = MAX_HOURS;
				lcd_setCursor(6, 0);
				fprintf(lcdout, "%u", currentTime.hour);
			} else {
				currentTime.hour--;
				if (currentTime.hour < MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "0%u", currentTime.hour);
				} else if (currentTime.hour >= MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "%u", currentTime.hour);
				}
			}

		}
		return RET_HANDLED;
		/* In case the rotary button is pressed*/
	case ROTARY_PRESSED_SIG:
		/* whenever the previous state is alarmEnabled*/
		if (fsm->isAlarmEnabled) {
			/*incrementing the alarm hours and adjusting the display and overflow of the readings*/
			if (alarmTime.hour >= MAX_HOURS) {
				alarmTime.hour = 0;
				lcd_setCursor(6, 0);
				fprintf(lcdout, "0%u", alarmTime.hour);
			} else {
				alarmTime.hour++;
				if (alarmTime.hour < MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "0%u", alarmTime.hour);
				} else if (alarmTime.hour >= MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "%u", alarmTime.hour);
				}
			}
		}

		/* whenever the previous state is the default*/
		else {
			/*incrementing the current hours and adjusting the display and overflow of the readings*/
			if (currentTime.hour >= MAX_HOURS) {
				currentTime.hour = 0;
				lcd_setCursor(6, 0);
				fprintf(lcdout, "0%u", currentTime.hour);
			} else {
				currentTime.hour++;
				if (currentTime.hour < MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "0%u", currentTime.hour);
				} else if (currentTime.hour >= MAX_HOURS_IN_UNITS) {
					lcd_setCursor(6, 0);
					fprintf(lcdout, "%u", currentTime.hour);
				}
			}

		}
		return RET_HANDLED;
		/* In case the joy stick button is pressed*/
	case JOY_STICK_PRESSED_SIG:
		/* move to the next state*/
		return TRANSITION(minutesSet);
	default:
		return RET_IGNORED;
	}
}

/* State for setting the minutes( State 3)*/
fsmReturnStatus minutesSet(Fsm *fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		return RET_HANDLED;
		/* In case the rotary encoder button is rotated CW*/
	case ROTARY_ENCODER_CW:
		/* whenever the previous state is alarmEnabled*/
		if (fsm->isAlarmEnabled) {
			/*incrementing the alarm minutes and adjusting the display and overflow of the readings*/
			if (alarmTime.minute >= MAX_MINUTES) {
				alarmTime.minute = 0;
				lcd_setCursor(11, 0);
				fprintf(lcdout, "0%u", alarmTime.minute);
			} else {
				alarmTime.minute++;
				if (alarmTime.minute < MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "0%u", alarmTime.minute);
				} else if (alarmTime.minute >= MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "%u", alarmTime.minute);
				}
			}
		}

		/* whenever the previous state is the default*/
		else {
			/*incrementing the current minutes and adjusting the display and overflow of the readings*/
			if (currentTime.minute >= MAX_MINUTES) {
				currentTime.minute = 0;
				lcd_setCursor(11, 0);
				fprintf(lcdout, "0%u", currentTime.minute);
			} else {
				currentTime.minute++;
				if (currentTime.minute < MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "0%u", currentTime.minute);
				} else if (currentTime.minute >= MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "%u", currentTime.minute);
				}
			}

		}
		return RET_HANDLED;
		/* In case the rotary encoder button is rotated CCW*/
	case ROTARY_ENCODER_CCW:
		/* whenever the previous state is alarmEnabled*/
		if (fsm->isAlarmEnabled) {
			/*decrementing the alarm minutes and adjusting the display and overflow of the readings*/
			if (alarmTime.minute <= 0) {
				alarmTime.minute = MAX_MINUTES;
				lcd_setCursor(11, 0);
				fprintf(lcdout, "%u", alarmTime.minute);
			} else {
				alarmTime.minute--;
				if (alarmTime.minute < MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "0%u", alarmTime.minute);
				} else if (alarmTime.minute >= MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "%u", alarmTime.minute);
				}
			}
		}

		/* whenever the previous state is the default*/
		else {
			/*decrementing the current minutes and adjusting the display and overflow of the readings*/
			if (currentTime.minute <= 0) {
				currentTime.minute = MAX_MINUTES;
				lcd_setCursor(11, 0);
				fprintf(lcdout, "%u", currentTime.minute);
			} else {
				currentTime.minute--;
				if (currentTime.minute < MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "0%u", currentTime.minute);
				} else if (currentTime.minute >= MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "%u", currentTime.minute);
				}
			}

		}
		return RET_HANDLED;
		/* In case the rotary button is pressed*/
	case ROTARY_PRESSED_SIG:
		/* whenever the previous state is alarmEnabled*/
		if (fsm->isAlarmEnabled) {
			/*incrementing the alarm minutes and adjusting the display and overflow of the readings*/
			if (alarmTime.minute >= MAX_MINUTES) {
				alarmTime.minute = 0;
				lcd_setCursor(11, 0);
				fprintf(lcdout, "0%u", alarmTime.minute);
			} else {
				alarmTime.minute++;
				if (alarmTime.minute < MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "0%u", alarmTime.minute);
				} else if (alarmTime.minute >= MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "%u", alarmTime.minute);
				}
			}
		}

		/* whenever the previous state is the default*/
		else {
			/*incrementing the current minutes and adjusting the display and overflow of the readings*/
			if (currentTime.minute >= MAX_MINUTES) {
				currentTime.minute = 0;
				lcd_setCursor(11, 0);
				fprintf(lcdout, "0%u", currentTime.minute);
			} else {
				currentTime.minute++;
				if (currentTime.minute < MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "0%u", currentTime.minute);
				} else if (currentTime.minute >= MAX_MINUTES_IN_UNITS) {
					lcd_setCursor(11, 0);
					fprintf(lcdout, "%u", currentTime.minute);
				}
			}

		}
		return RET_HANDLED;
		/* In case the joy stick button is pressed*/
	case JOY_STICK_PRESSED_SIG:
		/* whenever the alarm is enabled */
		if (fsm->isAlarmEnabled) {
			/*move to the next state */
			return TRANSITION(alarmRings);
		}
		/* whenever the alarm is not enabled*/
		else {
			/*start displaying the time by adding the related task to the scheduler */
			lcd_clear();
			lcd_setCursor(3, 0);
			if (currentTime.hour < MAX_HOURS_IN_UNITS
					&& currentTime.minute < MAX_MINUTES_IN_UNITS) {
				fprintf(lcdout, "0%u : 0%u : 00", currentTime.hour,
						currentTime.minute);
			} else if (currentTime.hour < MAX_HOURS_IN_UNITS
					&& currentTime.minute >= MAX_MINUTES_IN_UNITS) {
				fprintf(lcdout, "0%u : %u : %u", currentTime.hour,
						currentTime.minute, currentTime.second);

			} else if (currentTime.hour >= MAX_HOURS_IN_UNITS
					&& currentTime.minute < MAX_MINUTES_IN_UNITS) {
				fprintf(lcdout, "%u : 0%u : %u", currentTime.hour,
						currentTime.minute, currentTime.second);

			} else {
				fprintf(lcdout, "%u : %u : %u", currentTime.hour,
						currentTime.minute, currentTime.second);
			}
			currentTime.milli = 0;
			/* getting the current time*/
			scheduler_setTime(scheduler_getTime());
			scheduler_add(&displayTime);
			/* move to the next state */
			return TRANSITION(clockStart);
		}
	default:
		return RET_IGNORED;
	}
}

/* State for displaying the current time in HH : MM : SS  ( State 4 )*/
fsmReturnStatus clockStart(Fsm *fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		/*displaying a message to the user*/
		lcd_setCursor(2, 2);
		fprintf(lcdout, " clock started ");
		scheduler_add(&greenToggle);
		return RET_HANDLED;
		/*In case the rotary button is pressed*/
	case ROTARY_PRESSED_SIG:
		/*move to the next state */
		return TRANSITION(alarmEnabled);
	default:
		return RET_IGNORED;
	}
}

/* State representing the enabling of the alarm (state 5) */
fsmReturnStatus alarmEnabled(Fsm *fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		/*Turning on the yellow led as an indication for enabling the alarm and setting the "isAlarmEnabled" flag to true*/
		led_yellowOn();
		fsm->isAlarmEnabled = true;
		scheduler_remove(&displayTime);
		lcd_clear();
		lcd_setCursor(2, 2);
		fprintf(lcdout, " Alarm enabled ");
		return RET_HANDLED;
	case ROTARY_PRESSED_SIG:
		return TRANSITION(alarmDisabled);
	case JOY_STICK_PRESSED_SIG:
		return TRANSITION(hoursSet);
	default:
		return RET_IGNORED;
	}
}

/* State representing the disabling of the alarm (state 6) */
fsmReturnStatus alarmDisabled(Fsm *fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		/*resetting the alarm and its enabled flag*/
		lcd_setCursor(3, 0);
		if (currentTime.hour < MAX_HOURS_IN_UNITS
				&& currentTime.minute < MAX_MINUTES_IN_UNITS) {
			fprintf(lcdout, "0%u : 0%u : 00", currentTime.hour,
					currentTime.minute);
		} else if (currentTime.hour < MAX_HOURS_IN_UNITS
				&& currentTime.minute >= MAX_MINUTES_IN_UNITS) {
			fprintf(lcdout, "0%u : %u : %u", currentTime.hour,
					currentTime.minute, currentTime.second);

		} else if (currentTime.hour >= MAX_HOURS_IN_UNITS
				&& currentTime.minute < MAX_MINUTES_IN_UNITS) {
			fprintf(lcdout, "%u : 0%u : %u", currentTime.hour,
					currentTime.minute, currentTime.second);

		} else {
			fprintf(lcdout, "%u : %u : %u", currentTime.hour,
					currentTime.minute, currentTime.second);
		}
		scheduler_add(&displayTime);
		lcd_setCursor(2, 2);
		fprintf(lcdout, "Alarm disabled");
		alarmTime.hour = 0;
		alarmTime.minute = 0;
		led_yellowOff();
		fsm->isAlarmEnabled = false;
		return RET_HANDLED;
	case ROTARY_PRESSED_SIG:
		return TRANSITION(alarmEnabled);
	case JOY_STICK_PRESSED_SIG:
		return TRANSITION(hoursSet);
	default:
		return RET_IGNORED;
	}
}

/*State representing the alarm match and ringing it (State 7)*/
fsmReturnStatus alarmRings(Fsm *fsm, const Event *e) {
	switch (e->signal) {
	case ENTRY:
		scheduler_add(&triggerAlarm);
		return RET_HANDLED;
		/* In case any button is pressed or five seconds have passed , the alarm should stop ringing*/
	case JOY_STICK_PRESSED_SIG:
	case ROTARY_PRESSED_SIG:
	case FIVE_SECONDS_PASSED:
		/*removing the red toggling task from the scheduler*/
		led_redOff();
		lcd_clear();
		scheduler_remove(&triggerAlarm);
		return TRANSITION(alarmDisabled);
	default:
		return RET_IGNORED;
	}
}

int main(void) {
	/*
	 * Initializing the clock time with zeros
	 */
	currentTime.hour = 0;
	currentTime.minute = 0;
	currentTime.second = 0;
	currentTime.milli = 0;
	/*
	 * Initializing the alarm time with zeros
	 */
	alarmTime.hour = 0;
	alarmTime.minute = 0;
	alarmTime.second = 0;
	alarmTime.milli = 0;

	/*
	 * Initializing the leds
	 */
	led_yellowInit();
	led_greenInit();
	led_redInit();

	/*
	 * Initializing the Lcd
	 */
	lcd_init();

	/*
	 * Initializing the scheduler
	 */
	scheduler_init();

	/*
	 * Initializing the buttons
	 */
	button_init(false);

	/*
	 * Initializing the rotary encoder button
	 */

	rotary_init(false);

	/*Initializing the FSM and passing the initial state to start transitions*/
	fsm_init((Fsm*) &alarmClock, clockInit);

	/*
	 * Setting the call back function for the rotary button
	 */
	button_setRotaryButtonCallback(&rotaryPressedDispatch);
	/*
	 * Setting the call back function for the joy stick button
	 */
	button_setJoystickButtonCallback(&joystickPressedDispatch);
	/*
	 * Setting the call back function for the rotary encoder ( clock wise direction)
	 */
	rotary_setClockwiseCallback(&rotaryEncoderClockwiseDispatch);
	/*
	 * Setting the call back function for the rotary encoder ( counter clock wise direction)
	 */
	rotary_setCounterClockwiseCallback(&rotaryEncoderCounterclockwiseDispatch);

	while (1) {
		/*
		 * running the scheduler in a while loop
		 */
		scheduler_run();

	}
	return EXIT_SUCCESS;
}
