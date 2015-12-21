/*
 * main.c
 */
#include <stdio.h>
#include <stdlib.h>

#include "generalinfo.h"
#include "Queue.h"
#include "memory.h"
#include "PriorityQueue.h"
#include "uart.h"
#include "systick.h"
#include "PendSVHandler.h"
#include "clock.h"
#include "cursor.h"
#include "Kernel.h"
#include "ControlSoftware.h"
#include "PcbQueue.h"
#include "thread.h"

#include "testcase.h"

void init()
{
	initialization_Clock();
	initial_memory();
	initialize_Queue();
	InitializePriorityQueueManagement();
	initialization_uart();

	NVIC_SYS_PRI3_R |= PENDSV_LOWEST_PRIORITY;
	Configuration_and_Control |= BASETHR;

	initialization_Control_Software();
	ENABLE_GLOBAL_INTERRUPT();

	RegistrationProcess(IldeQrocess,0);
}

int main(void)
{
	init();

	RegistrationProcess(printJ,2);
	RegistrationProcess(printC,2);
//	RegistrationProcess(printA,1);

	SVC();
	return 0;
}
