/*
 * ControlSoftware.c
 *
 *  Created on: Oct 14, 2015
 *      Author: Duc Cuong Dinh
 */
#include <stdio.h>
#include <stdlib.h>

#include "Queue.h"
#include "ControlSoftware.h"
#include "memory.h"
#include "uart.h"
#include "cursor.h"
#include "clock.h"
#include "Kernel.h"
#include "PcbQueue.h"
#include "thread.h"

//#define testing_cs

//global variables
//buffer for transmit: the current transmitting
volatile char *clock_TX;
//the buffer length of buffer_ready
volatile unsigned int clock_length;

//command state:
volatile enum COMMAND_STATE current_state;

//other information:
//infmation for clock:
volatile unsigned int hour;
volatile unsigned int minute;
volatile unsigned int second;
volatile unsigned int milisecond;
//how many char is pressed
volatile unsigned int char_input;

extern unsigned int state_stopwatch;
//inialization for Control Software

void initialization_Control_Software(void)
{
	clock_TX  = NULL;
	current_state = VALID;
	hour = 0;
	minute = 0;
	second = 0;
	milisecond = 0;
	char_input = 15;
}

//for testing
//print out the every single char in buffer ready to make sure everything is ready
void print_st(void)
{
	unsigned int i;
	for(i=0;i<clock_length;i++)
		printf("(%d %c)",i,clock_TX[i]);
	printf("\n");
}

//get_received_char:
//when a character is press, the command and the char is stored in the queue
//when dequeueing the queue, the command and the data will be burst
//it is considered whether it is valid or invalid by sequence typing
//the invalid state will be reset when enter key is hit
unsigned int get_received_char(unsigned int data)
{
	char_input++;
	state_stopwatch = FALSE;

	if(clock_TX == NULL)
	{
		clock_TX = (volatile char*) allocate(STRING_SIZE);
		clock_length = 0;
	}
	switch(data)
	{
		case '\r':
			cursor_position(clock_TX,&clock_length,LINE_COMMAND,COLUMN);
			cursor_erase(clock_TX,&clock_length);
			switch(current_state)
			{
				case STATE_O_2:
					set_stopclock_status(TRUE);
					break;
				case STATE_E_4:
					get_mainclock();
					break;

				case STATE_MIL_1:
					set_timing(hour,minute,second,milisecond);
					break;
				case STATE_W_1:
					print__id(getId());
					break;
				case STATE_X_1:
					print__id(promoteby(1));
					break;
				case STATE_Y_1:
					print__id(demoteby(1));
					break;
				case STATE_Z_1:
					print__id(terminate());
					break;
				case STATE_J_1:
					print__id(RegistrationProcess(printJ,1));
					break;
				case STATE_C_1:
					print__id(RegistrationProcess(printC,1));
					break;
				default:
					break;
			}
			char_input = 15;
			current_state = VALID;
			break;
		case ' ':
			current_state = (current_state == STATE_T_3) ? STATE_SPACE_4 : INVALID;
			break;
		case '0':
		case '1':
		case '2':
			switch(current_state)
			{
				case STATE_SPACE_4:
					current_state = STATE_HOUR_1;
					hour = (data - '0') * BASE_NUMBER_10;
					break;
				case STATE_HOUR_1:
					current_state = STATE_HOUR_2;
					hour += (data - '0');
					break;
				case STATE_HOUR_2:
					current_state = STATE_MIN_1;
					minute = (data - '0') * BASE_NUMBER_10;
					break;
				case STATE_MIN_1:
					current_state = STATE_MIN_2;
					minute += (data - '0');
					break;
				case STATE_MIN_2:
					current_state = STATE_SEC_1;
					second = (data - '0') * BASE_NUMBER_10;
					break;
				case STATE_SEC_1:
					current_state = STATE_SEC_2;
					second += (data - '0');
					break;
				case STATE_SEC_2:
					current_state = STATE_MIL_1;
					milisecond = (data - '0');
					break;
				default:
					current_state = INVALID;
					break;
			}
			break;
		case '3':
		case '4':
		case '5':
			switch(current_state)
			{
				case STATE_HOUR_1:
					hour += (data - '0');
					current_state = (hour < MAX_HOUR) ? STATE_HOUR_2 : INVALID;
					break;
				case STATE_HOUR_2:
					current_state = STATE_MIN_1;
					minute = (data - '0') * BASE_NUMBER_10;
					break;
				case STATE_MIN_1:
					current_state = STATE_MIN_2;
					minute += (data - '0');
					break;
				case STATE_MIN_2:
					current_state = STATE_SEC_1;
					second = (data - '0') * BASE_NUMBER_10;
					break;
				case STATE_SEC_1:
					current_state = STATE_SEC_2;
					second += (data - '0');
					break;
				case STATE_SEC_2:
					current_state = STATE_MIL_1;
					milisecond = (data - '0');
					break;
				default:
					current_state = INVALID;
					break;
			}
			break;
		case '6':
		case '7':
		case '8':
		case '9':
			switch(current_state)
			{
				case STATE_HOUR_1:
					hour += (data - '0');
					current_state = (hour <MAX_HOUR) ? STATE_HOUR_2 : INVALID;
					break;
				case STATE_MIN_1:
					minute += (data - '0');
					current_state = (minute < MAX_MINUTE) ? STATE_MIN_2 : INVALID;
					break;
				case STATE_SEC_1:
					second += (data - '0');
					current_state = (second < MAX_SECOND) ? STATE_SEC_2 : INVALID;
					break;
				case STATE_SEC_2:
					current_state = STATE_MIL_1;
					milisecond = (data - '0');
					break;
				default:
					current_state = INVALID;
					break;
			}
			break;
		case 'C':
			current_state = (current_state == VALID) ? STATE_C_1 : INVALID;
			break;
		case 'E':
			current_state = (current_state == STATE_M_3) ? STATE_E_4 : ((current_state == STATE_S_1) ? STATE_E_2 : INVALID);
			break;
		case 'G':
			current_state = (current_state == VALID) ? STATE_G_1 : INVALID;
			break;
		case 'I':
			current_state = (current_state == STATE_T_1) ? STATE_I_2 : INVALID;
			break;
		case 'J':
			current_state = (current_state == VALID) ? STATE_J_1 : INVALID;
			break;
		case 'M':
			current_state = (current_state == STATE_I_2) ? STATE_M_3 : INVALID;
			break;
		case 'O':
			current_state = (current_state == STATE_G_1) ? STATE_O_2 : INVALID;
			break;
		case 'S':
			current_state = (current_state == VALID) ? STATE_S_1 : INVALID;
			break;
		case 'T':
			current_state = (current_state == VALID) ? STATE_T_1 : ((current_state == STATE_E_2) ? STATE_T_3: INVALID);
			break;
		case 'W':
			current_state = (current_state == VALID) ? STATE_W_1 : INVALID;
			break;
		case 'X':
			current_state = (current_state == VALID) ? STATE_X_1 : INVALID;
			break;
		case 'Y':
			current_state = (current_state == VALID) ? STATE_Y_1 : INVALID;
			break;
		case 'Z':
			current_state = (current_state == VALID) ? STATE_Z_1 : INVALID;
			break;
		case 'c':
			current_state = (current_state == VALID) ? STATE_C_1 : INVALID;
			break;
		case 'e':
			current_state = (current_state == STATE_M_3) ? STATE_E_4 : ((current_state == STATE_S_1) ? STATE_E_2 : INVALID);
			break;
		case 'g':
			current_state = (current_state == VALID) ? STATE_G_1 : INVALID;
			break;
		case 'i':
			current_state = (current_state == STATE_T_1) ? STATE_I_2 : INVALID;
			break;
		case 'j':
			current_state = (current_state == VALID) ? STATE_J_1 : INVALID;
		case 'm':
			current_state = (current_state == STATE_I_2) ? STATE_M_3 : INVALID;
			break;
		case 'o':
			current_state = (current_state == STATE_G_1) ? STATE_O_2 : INVALID;
			break;
		case 's':
			current_state = (current_state == VALID) ? STATE_S_1 : INVALID;
			break;
		case 't':
			current_state = (current_state == VALID) ? STATE_T_1 : ((current_state == STATE_E_2) ? STATE_T_3: INVALID);
			break;
		case 'w':
			current_state = (current_state == VALID) ? STATE_W_1 : INVALID;
			break;
		case 'x':
			current_state = (current_state == VALID) ? STATE_X_1 : INVALID;
			break;
		case 'y':
			current_state = (current_state == VALID) ? STATE_Y_1 : INVALID;
			break;
		case 'z':
			current_state = (current_state == VALID) ? STATE_Z_1 : INVALID;
			break;
		default:
			current_state = INVALID;
			break;
	}
	generate_command_string(data);

	clock_TX[clock_length++] = (char)NULL_INDEX;//the end of string
	send_clock_to_uart(1);
	return FALSE;
}

//Once the clock is calculated the total 1/10 second, it will call this function
//to through the data: hour, minute, second, millisecond
//First: the function getting the data
//Second: generating the clock_TX
void get_clock(unsigned int hour_, unsigned int minute_, unsigned int second_, unsigned int milisecond_)
{
	hour = hour_;
	minute = minute_;
	second = second_;
	milisecond = milisecond_;

	generate_clock_string();
}

//convert information of clock into string
void convert_clock_to_string(void)
{
	clock_TX[clock_length++] = (hour / BASE_NUMBER_10) + '0';
	clock_TX[clock_length++] = (hour % BASE_NUMBER_10) + '0';
	clock_TX[clock_length++] = ':';
	clock_TX[clock_length++] = (minute / BASE_NUMBER_10) + '0';
	clock_TX[clock_length++] = (minute % BASE_NUMBER_10) + '0';
	clock_TX[clock_length++] = ':';
	clock_TX[clock_length++] = (second / BASE_NUMBER_10) + '0';
	clock_TX[clock_length++] = (second % BASE_NUMBER_10) + '0';
	clock_TX[clock_length++] = '.';
	clock_TX[clock_length++] = milisecond + '0';
	clock_TX[clock_length++] = '\0';
}

//generating the clock information and stored in clock_TX
void generate_clock_string(void)
{
	if(clock_TX==NULL)
	{
		clock_TX = (volatile char*) allocate(STRING_SIZE);
		clock_length = 0;
	}
	if(clock_TX!=NULL)
	{
		//adding postion
		cursor_position(clock_TX, &clock_length, LINE_CLOCK, COLUMN);
		//adding clock infor
		convert_clock_to_string();

		clock_TX[clock_length++] = (char)NULL_INDEX;//the end of string
	}
}

//generating the echo of typing
void generate_command_string(char c)
{
	if(clock_TX != NULL)
	{
		if(clock_length>0 && (clock_TX[clock_length++] == (char)NULL_INDEX))
			clock_length--;

		//adding postion
		cursor_position(clock_TX, &clock_length, LINE_COMMAND, char_input);
		//if the char input is not enter key
		if(c != '\r')
		{
			//add char to the string
			clock_TX[clock_length++] = c;
			//if the current state is state_hour_2 or state_minute_2
			//then add another char ":" right after the key typed
			if(current_state==STATE_HOUR_2 || current_state==STATE_MIN_2)
			{
				char_input++;
				clock_TX[clock_length++] = ':';
			}
			//if the current state is state_sec_2
			if(current_state==STATE_SEC_2)
			{
				char_input++;
				clock_TX[clock_length++] = '.';
			}
		}
		else
			cursor_erase(clock_TX, &clock_length);
	}
}

//print_ID is used to print process's ID to terminal
void print__id(unsigned int id)
{
	if(clock_TX==NULL)
	{
		clock_TX = (volatile char*) allocate(STRING_SIZE);
		clock_length = 0;
	}
	//making the string
	if(clock_TX!=NULL)
	{
		clock_TX[clock_length++] = 'I';
		clock_TX[clock_length++] = 'D';
		clock_TX[clock_length++] = ':';
		clock_TX[clock_length++] = ' ';
		convert_to_string(clock_TX, &clock_length,id);
		clock_TX[clock_length++] = (char)NULL_INDEX;//the end of string
	}

}

//send the string to UART
//in the string should have cursor commands,
//and in the end, the char 0xFF (NULL_INDEX) is indecated it is the end of string
unsigned int send_clock_to_uart(unsigned int data)
{
	clock_TX[clock_length++] = (char)NULL_INDEX;//the end of string
	send_to_UART(clock_TX);
	clock_TX = NULL;
	return FALSE;
}
