/*
 * thread.c
 *
 *  Created on: Nov 17, 2015
 *      Author: Duc Cuong Dinh
 */
#include <stdio.h>
#include "cursor.h"
#include "uart.h"
#include "Kernel.h"
#include "generalinfo.h"
#include "memory.h"
#include "thread.h"

unsigned int IldeQrocess(unsigned int data)
{
	while(1);
	return TRUE;
}

unsigned int printJ(unsigned int data)
{
	char *a1;
	unsigned int location;
	static unsigned char count;
	count++;
	count = count %10;
	count = count + '0';
	location = 0;
	while(1)
	{
		printf("<J>\n");
		a1 = (char*) allocate(STRING_SIZE);
		if(a1!=NULL)
		{
			location = 0;
			cursor_position(a1, &location, THREAD_1, 1);
			a1[location++] = 'J';
			a1[location++] = 'e';
			a1[location++] = 'n';
			a1[location++] = 'n';
			a1[location++] = 'y';
			a1[location++] = count;
			a1[location++] = NUL;
			a1[location++] = NULL_INDEX;
			count =count - '0';
			count++;
			count = count %10;
			count = count + '0';
			send_to_UART(a1);
		}
	}
	return TRUE;
}

unsigned int printC(unsigned int data)
{
	static unsigned char count;
	char *b1;
	unsigned int location;
	count++;
	count = count %10;
	count = count + '0';
	location = 0;
	while(1)
	{
		printf("<C>\n");
		b1 = (char*)allocate(STRING_SIZE);
		if(b1!=NULL)
		{
			location = 0;
			cursor_position(b1, &location, THREAD_2, 1);
			b1[location++] = 'C';
			b1[location++] = 'u';
			b1[location++] = 'o';
			b1[location++] = 'n';
			b1[location++] = 'g';
			b1[location++] = count;
			b1[location++] = NUL;
			b1[location++] = NULL_INDEX;
			count =count - '0';
			count++;
			count = count %10;
			count = count + '0';
			send_to_UART(b1);
		}
	}
	return TRUE;
}

unsigned int printA(unsigned int data)
{
	char *a1;
	unsigned int location;
	unsigned int i;
	static unsigned char count;
	count++;
	count = count %10;
	count = count + '0';
	location = 0;

	for(i=0;i<100;i++)
	{
		printf("<A>\n");
		a1 = (char*) allocate(STRING_SIZE);
		if(a1!=NULL)
		{
			location = 0;
			cursor_position(a1, &location, THREAD_3, 1);
			a1[location++] = 'A';
			a1[location++] = 'n';
			a1[location++] = 'd';
			a1[location++] = 'r';
			a1[location++] = 'e';
			a1[location++] = 'w';
			a1[location++] = count;
			a1[location++] = NUL;
			a1[location++] = NULL_INDEX;
			count =count - '0';
			count++;
			count = count %10;
			count = count + '0';
			send_to_UART(a1);
		}
	}
	return TRUE;
}
