/*
 * Semaphore list handling functions
*/
#ifndef ASL_H
#define ASL_H

// Preprocessing directives
#include "../h/pcb.h"
#include "../h/const.h"
#include "../h/types.h"

// Function declarations
// [internal functions]
HIDDEN inline int isEmpty(semd_t *header);
HIDDEN inline void addToASL(semd_t *sem, int *semAdd);
HIDDEN inline void addToSemdFree(semd_t *sem);
HIDDEN inline semd_t *removeFromSemdFree(void);
HIDDEN inline semd_t *findSemaphore(int *semAdd);
HIDDEN inline semd_t *freeSemaphore(int *semAdd);
// [external functions]
EXTERN void initASL(void);
EXTERN int insertBlocked(int *semAdd, pcb_t *p);
EXTERN pcb_t *removeBlocked(int *semAdd);
EXTERN pcb_t *outBlocked(pcb_t *p);
EXTERN pcb_t *headBlocked(int *semAdd);

// Pointer to the head of the Active Semaphore List (ASL)
static semd_t *semd_h;

// Pointer to the head of the unused semaphore descriptors list
static semd_t *semdFree_h;

/*
 * Check if a list is empty
 * Input:	dummy header of a list
 * Output:	TRUE, if the list is empty
 * 			FALSE, else
*/
HIDDEN inline int isEmpty(semd_t *header)
{
	return header->s_next? FALSE : TRUE;
}

/*
 * Add a semaphore to ASL
 * Input:	semd_t *sem,	pointer to the semaphore
 * 			int *semAdd,	physical address of the semaphore
 * Output:	void
*/
HIDDEN inline void addToASL(semd_t *sem, int *semAdd)
{
	semd_t *it;

	// Iterate ASL until
	for(it = semd_h;
		// the end of ASL is reached OR
		it->s_next &&
		// a semaphore with higher value is found
		it->s_next->s_semAdd < semAdd;
		it = it->s_next);

	sem->s_next = it->s_next;
	it->s_next = sem;
}

/*
 * Add a semaphore to the top of semdFree list
 * Input:	semd_t *sem, pointer to a semaphore in ASL
 * Output:	void
*/
HIDDEN inline void addToSemdFree(semd_t *sem)
{
	sem->s_next = semdFree_h->s_next;
	semdFree_h->s_next = sem;
}

/*
 * Removes a semaphore from the top of semdFree
 * Input:	void
 * Output:	semd_t *, pointer to a free semaphore
*/
HIDDEN inline semd_t *removeFromSemdFree(void)
{
	semd_t *output;

	// Pre-conditions: semdFree is not empty
	if(isEmpty(semdFree_h)) return NULL;

	output = semdFree_h->s_next;
	semdFree_h->s_next = semdFree_h->s_next->s_next;
	output->s_next = NULL;
	output->s_procQ = mkEmptyProcQ();

	return output;
}

/*
 * Search a given semaphore in ASL
 * Input:	int *semAdd, 	physical address of the semaphore
 * Output:	semd_t *,		pointer to the previous node
*/
HIDDEN inline semd_t *findSemaphore(int *semAdd)
{
	semd_t *it;

	// Iterate ASL until
	for(it = semd_h;
		// the end of ASL is reached OR
		it->s_next &&
		// the semaphore is found
		it->s_next->s_semAdd != semAdd;
		it = it->s_next);

	return it;
}

/*
 * Initialize the semdFree list to contain all the elements of the
 * array static semd_t semdTable[MAXPROC + 1].
 * The size is increased by one because of the dummy header.
 * This method will be only called once during data structure
 * initialization.
*/
EXTERN void initASL(void)
{
	static semd_t semdTable[MAXPROC + 1];
	int i;

	for(i = 0; i < MAXPROC; i++)
	{
		semdTable[i].s_next = &semdTable[i + 1];
	}

	semdTable[MAXPROC].s_next = NULL;
	semdFree_h = &semdTable[0];
	semd_h = NULL;
}

/*
 * Insert the ProcBlk pointed to by p at the tail of the process queue
 * associated with the semaphore whose physical address is semAdd
 * and set the semaphore address of p to semAdd.
 * If the semaphore is currently not active (i.e. there is no descriptor
 * for it in the ASL), allocate a new descriptor from the semdFree list,
 * insert it in the ASL (at the appropriate position), initialize all of
 * the fields (i.e. set s_semAdd to semAdd, and s_procq to mkEmptyProcQ()),
 * and proceed as above.
 * If a new semaphore descriptor needs to be allocated and the semdFree
 * list is empty, return TRUE.
 * In all other cases return FALSE.
*/
EXTERN int insertBlocked(int *semAdd, pcb_t *p)
{
	int output;
	semd_t *sem;

	// Pre-conditions: semAdd and p are not NULL
	if(!semAdd || !NULL) return FALSE;

	sem = findSemaphore(semAdd);
	// [Case 1] semAdd is active
	if(sem->s_next)
	{
	 	p->p_semAdd = semAdd;
	 	insertProcQ(&(sem->s_next->s_procQ), p);
	 	output = FALSE;
	}
	// [Case 2] semAdd is not active
	else
	{
		sem = removeFromSemdFree();
		// [Case 2.1] semdFree is not empty
		if(sem)
		{
			addToASL(sem, semAdd);
			output = FALSE;
		}
		// [Case 2.2] semdFree is empty
		else
		{
			output = TRUE;
		}
	}

	return output;
}

/*
 * Search the ASL for a descriptor of this semaphore.
 * If none is found, return NULL; otherwise, remove the first
 * (i.e. head) ProcBlk from the process queue of the found
 * semaphore descriptor and return a pointer to it.
 * If the process queue for this semaphore becomes empty
 * (emptyProcQ(s_procq) is TRUE), remove the semaphore
 * descriptor from the ASL and return it to the semdFree list.
*/
EXTERN pcb_t *removeBlocked(int *semAdd)
{
	pcb_t *output;
	semd_t *sem;

	// Pre-conditions: semAdd is not NULL
	if(!semAdd) return NULL;

	sem = findSemaphore(semAdd);
	// [Case 1] semAdd is in ASL
	if(sem->s_next)
	{
		output = removeProcQ(&(sem->s_next->s_procQ));

		// [SubCase] ProcQ is now empty
		if(emptyProcQ(sem->s_next->s_procQ))
		{
			sem->s_next = sem->s_next->s_next;
			addToSemdFree(sem->s_next);
		}
	}
	// [Case 2] semaphore is not in ASL
	else
	{
		output = NULL;
	}

	return output;
}

/*
 * Remove the ProcBlk pointed to by p from the process queue
 * associated with p’s semaphore (p->p_semAdd) on the ASL.
 * If ProcBlk pointed to by p does not appear in the process
 * queue associated with p’s semaphore, which is an error
 * condition, return NULL; otherwise, return p.
*/
EXTERN pcb_t *outBlocked(pcb_t *p)
{
	pcb_t *output;
	semd_t *sem;

	// Pre-conditions: p is not NULL
	if(!p) return NULL;

	sem = findSemaphore(p->p_semAdd);
	// [Case 1] semAdd is in ASL
	if(sem->next)
	{
		output = outProcQ(&(sem->s_procQ), p);
	}
	// [Case 2] semAdd is not in ASL
	else
	{
		output = NULL;
	}

	return output;
}

/*
 * Return a pointer to the ProcBlk that is at the head of the
 * process queue associated with the semaphore semAdd.
 * Return NULL if semAdd is not found on the ASL or if the
 * process queue associated with semAdd is empty.
*/
EXTERN pcb_t *headBlocked(int *semAdd)
{
	pcb_t *output;
	semd_t *sem;

	// Pre-conditions: semAdd is not NULL
	if(!semAdd) return NULL;

	sem = findSemaphore(semAdd);
	// [Case 1] semAdd is in ASL
	if(sem->next)
	{
		output = headProcQ(sem->s_procQ);
	}
	// [Case 2] semAdd is not in ASL
	else
	{
		output = NULL;
	}

	return output;
}
#endif
