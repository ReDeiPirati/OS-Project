#ifndef INITIAL_H
#define INITIAL_H

/**
@file initial.h
@author
@note main().
*/

#include "../../phase1/h/pcb.h"
#include "exceptions.h"
#include "scheduler.h"
#include "libumps.h"
#include "interrupts.h"

/* Global variables declarations */
struct list_head readyQueue;							/**< Process ready queue */
pcb_t *currentProcess;									/**< Pointer to the executing PCB */
U32 processCount;										/**< Process counter */
U32 pidCount;											/**< PID counter */
U32 softBlockCount;										/**< Blocked process counter */
int statusWordDev[STATUS_WORD_ROWS][STATUS_WORD_COLS];	/**< Device Status Word table */
struct													/**< Semaphore structure */
{
	int disk[DEV_PER_INT];
	int tape[DEV_PER_INT];
	int network[DEV_PER_INT];
	int printer[DEV_PER_INT];
	int terminalR[DEV_PER_INT];
	int terminalT[DEV_PER_INT];
} sem;
int pseudo_clock;										/**< Pseudo-clock semaphore */
cpu_t processTOD;										/**< Process start time */
int timerTick;											/**< Tick timer */
cpu_t startTimerTick;									/**< Pseudo-clock tick start time */
pcb_pid_t pcbused_table[MAXPROC];						/**< Used PCB table */

extern void test(void);

/**
  * @brief Populate a new processor state area.
  * @param area Physical address of the area.
  * @param handler Physical address of the handler.
  * @return Void.
 */
HIDDEN void populate(memaddr area, memaddr handler);

/**
@brief The entry point for Kaya which performs the nucleus initialization.
@return Void.
*/
void main(void);

#endif
