/*INCLUDES ************************************************************/
#include "ses_timer.h"
#include "ses_scheduler.h"
#include "ses_led.h"
#include "ses_lcd.h"

/* DEFINES & MACROS **********************************************************/

//Pointer to taskDescriptor structure always pointing to the first task in the list
static taskDescriptor* taskList = NULL;

//Pointer to taskDescriptor structure used as an index in the scheduler_update function
static taskDescriptor* taskListUpdate = NULL;

static taskDescriptor* taskListIndex = NULL;

static taskDescriptor* temp = NULL;

static taskDescriptor* pointer = NULL;

/*A variable of type systemTime_t to increment the current time set by the user*/

systemTime_t tick = 0;

/*FUNCTIONS DEFINITION *************************************************/

/*void scheduler_update(void)
 *
 * @param void ---> scheduler_update takes void
 *
 * @return void---> scheduler_update returns void
 *
 *--->taskListUpdate is a pointer to the first task in the linked list ,
 *where it traverses the list till it reaches "NULL" and check for the expire time of each task , when it reaches "0" ,
 *its flag is marked for execution and after that the frequency of execution depends on the period
 *--->the access to the list of taskDescriptors has to be restricted by disabling the interrupts in critical sections (adding Atomic block )
 */
static void scheduler_update(void) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		taskListUpdate = taskList;
		while (taskListUpdate != NULL) {
			taskListUpdate->expire--;
			if (taskListUpdate->expire == 0) {
				taskListUpdate->execute = 1;
				if (taskListUpdate->period > 0) {
					taskListUpdate->expire = taskListUpdate->period;
				}
			}
			taskListUpdate = taskListUpdate->next;
		}
	}
}

/*
 * void scheduler_init()
 *
 * @param void ---> scheduler_init takes void
 *
 * @return void---> scheduler_init returns void
 *
 *---> timer 2 is initialized where it controls the scheduler update using the timer call back function
 *
 */
void scheduler_init() {

	timer2_start();
	timer2_setCallback(scheduler_update);
}

/*
 * void scheduler_run()
 *
 * @param void ---> scheduler_run takes void
 *
 * @return void---> scheduler_run returns void
 *
 * --- > the "scheduler_run" function traverses periodically polls for executable tasks according to their execution flag
 * (from "scheduler_update" function ) and checks for the period of execution and removes it if it is a non-periodic task.
 *
 * --->the access to the list of taskDescriptors has to be restricted by disabling the interrupts in critical sections (adding Atomic block )
 */

void scheduler_run() {
	taskListIndex = taskList;
	while (taskListIndex != NULL) {

		if (taskListIndex->execute == 1 && taskListIndex->period == 0) {
			taskListIndex->task(taskListIndex->param);
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				taskListIndex->execute = 0;
			}
			scheduler_remove(taskListIndex);
		} else if (taskListIndex->execute == 1 && taskListIndex->period != 0) {

			taskListIndex->task(taskListIndex->param);
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				taskListIndex->execute = 0;
			}
		}
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			taskListIndex = taskListIndex->next;
		}
	}
}

/*
 * bool scheduler_add(taskDescriptor * toAdd)
 *
 * @param taskDescriptor* toAdd ---> scheduler_add takes a pointer to the taskDescriptor structure to be added to the linked list
 *
 * @return void---> scheduler_add returns whether the task is successfully added or not.
 *
 * ---> the "scheduler_add" function checks for different things :-
 * 1) if the taskList is empty and no added task is passed -->returns false
 * 2) if the taskList is empty and an added task is passed -->toAdd is inserted in the taskList and the function returns true
 * 3) if the taskList is not empty and we want to add different tasks --> a temp pointer to the linked list is defined which traverses the list till it reaches "NULL"
 *    and then the task is added and in return it points to "NULL" as a next task.
 *
 *--->the access to the list of taskDescriptors has to be restricted by disabling the interrupts in critical sections (adding Atomic block )
 */

bool scheduler_add(taskDescriptor * toAdd) {

	bool added = true;

	if ((taskList == NULL) && (toAdd == NULL)) {
		added = false;
	} else if ((taskList == NULL) && (toAdd != NULL)) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			taskList = toAdd;
		}
		added = true;
	} else {
		taskDescriptor* tem = taskList;
		while (tem->next != NULL) {

			if ((toAdd == NULL) || (tem == toAdd)) {
				added = false;
			} else {
				ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
				{
					tem = tem->next;
				}
			}
		}
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			tem->next = toAdd;
		}

		added = true;
	}

	return added;

}
/*
 * void scheduler_remove(taskDescriptor * toRemove)
 *
 * @param taskDescriptor* toRemove ---> scheduler_remove takes a pointer to the taskDescriptor structure to be removed from the linked list
 *
 * @return void---> scheduler_remove returns void and checks the following :-
 *
 * 1)if task list has only one task and the task should be removed
 * 2)if task list has more than one task and the first task should be removed
 * 3)if task list has more than one task and the task that should be removed is not the first task in the list

 * --->the access to the list of taskDescriptors has to be restricted by disabling the interrupts in critical sections (adding Atomic block )
 */
void scheduler_remove(taskDescriptor * toRemove) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		temp = taskList;
	}

	if (temp->next == NULL && (temp == toRemove))
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			taskList = NULL;
		}

	else if (temp == toRemove) {
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			pointer = temp->next;
			temp->next = NULL;
			temp = pointer;
			taskList = temp;
		}

	} else {
		while (temp->next != toRemove && temp->next != NULL) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				temp = temp->next;
			}
		}
		ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		{
			pointer = (temp->next)->next;
			(temp->next)->next = NULL;
			temp->next = pointer;

		}
	}
}

systemTime_t scheduler_getTime() {

	systemTime_t hours = currentTime.hour * 3600000;
	systemTime_t minutes = currentTime.minute * 60000;
	systemTime_t seconds = currentTime.second * 1000;
	systemTime_t millis = currentTime.milli;
	return (hours + minutes + seconds + millis);

}

void scheduler_setTime(systemTime_t time) {

	tick = time;

}
