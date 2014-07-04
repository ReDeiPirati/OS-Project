#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/**
@file exceptions.h
@author
@note This module implements the TLB PgmTrap and SYS/Bp exception handlers.
*/

#include "../../phase1/h/pcb.h"
#include "initial.h"
#include "interrupts.h"
#include "scheduler.h"
#include "libuarm.h"

/**
@brief Save current processor state into a new state.
@param current Current state.
@param new New state.
@return Void.
*/
HIDDEN void saveCurrentState(state_t *currentState, state_t *newState);

/**
@brief (SYS1) Create a new process.
@param statep Initial state for the newly created process.
@return -1 in case of error; 0 in case of success.
*/
int createProcess(state_t *statep);

/**
@brief (SYS2) Terminate the executing process as well as all its progeny.
@return Void.
*/
int terminateProcess(int pid);

/**
@brief (SYS3) Perform a V operation on a semaphore.
@param semaddr Physical address of the semaphore.
@return Void.
*/
void verhogen(int *semaddr);

/**
@brief (SYS4) Perform a P operation on a semaphore.
@param semaddr Physical address of the semaphore.
@return Void.
*/
void passeren(int *semaddr);

/**
@brief (SYS6) Get the amount of processor time used by the calling process.
@return Amount of processor time.
*/
cpu_t getCPUTime();

/**
@brief (SYS7) Perform a P operation on the pseudo-clock timer semaphore.
@return Void.
*/
void waitClock();

/**
@brief (SYS8) Perform a P operation on the semaphore that the nucleus maintains for the I/O device.
@param intlNo Interrupt line number.
@param dnum Device number.
@param waitForTermRead TRUE if waiting for a terminal read operation; FALSE otherwise.
@return The device's Status Word.
*/
unsigned int waitIO(int intlNo, int dnum, int waitForTermRead);

#endif
