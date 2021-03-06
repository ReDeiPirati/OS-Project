/*
@file dependencies.e
@brief External definitions for initial.c, scheduler.c, exceptions.c, interrupts.c
*/

#include "../../include/uARMconst.h"
#include "../../include/uARMtypes.h"
#include "../../include/libuarm.h"
#include "../../include/arch.h"
#include "../../include/base.h"
#include "../../include/types.h"
#include "../../include/const.h"

/* Phase 1 */
#include "../../phase1/e/pcb.e"
#include "../../phase1/e/asl.e"

/* Phase 2 */
#include "initial.e"
#include "scheduler.e"
#include "exceptions.e"
#include "interrupts.e"