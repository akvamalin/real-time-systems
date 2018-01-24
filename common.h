/*
	Yevhenii Maliavka
    BAIN5
	Hochschule Merseburg
	********************************************************
	This module contains common custom defines and functions
	that are/can be used in several task implementations
	********************************************************
	TODO: split into interface and src
*/

#ifndef COMMON_H
#define COMMON_H

#define UBYTE INT8U
#define KEY_ESC 27
#define KEY_SPACE 32
#define DEFAULT_COLOUR DISP_FGND_WHITE + DISP_BGND_BLACK
#define wait(seconds)  OSTimeDlyHMSM(0, 0, seconds, 0)
#define waitMili(miliseconds)  OSTimeDlyHMSM(0, 0, 0, miliseconds)
#define tick(times) OSTimeDly(times)
#define print(x, y, msg) PC_DispStr(x, y, msg, DEFAULT_COLOUR)
#define EMPTY_STRING "                                                                    "
#define STATUS_POS_X 0
#define STATUS_POS_Y 0
#define STATUS_WAIT_TIME 2

#include  "includes.h"
#include <stdarg.h>

//use this function instead of print define
void printy(int x, int y, const char* format, ...){
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	print(x, y, buffer);
	va_end(args);
}

void status(const char* format, ...){
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	printy(STATUS_POS_X, STATUS_POS_Y, "STATUS: %s", buffer);
	va_end(args);
	wait(STATUS_WAIT_TIME);
	printy(STATUS_POS_X, STATUS_POS_Y, EMPTY_STRING);
}

// Priority functionality
// backwards compatible
// the old modules use built-in freePrio
// and getFreePrio. The first available
// free priority is 5, the higher priorities
// are already in system use
byte nextFreePrio = 5;

byte getNextFreePrio(){
    return nextFreePrio++;
}

void errorHandler(char *str, UBYTE retnum, UBYTE returnOS){
	char s[100];
	sprintf(s, "%s %5d", str, retnum);
	print(0, 21, s);
	wait(4);
	if(returnOS){
	    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);
        exit(1);
	}
	else{
	    print(0, 21, EMPTY_STRING);
	}
}

void SemPendSafe(OS_EVENT* semaphore, int timeout){
	UBYTE err;
	OSSemPend(semaphore, timeout, &err);
	if(err != 0){
		return errorHandler("Error pending semaphore: %d", err, 1);
	}
}


void createTask(void* func, void* data, void* stack, byte prio){
    int status;
    status = OSTaskCreate(func, data, stack, prio);
    if(status != OS_ERR_NONE){
        errorHandler("ERROR: Error while creating a task:", status, 1);
    }
}

#endif /* COMMON_H */