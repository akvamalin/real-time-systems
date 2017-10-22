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
#define KEY_ESC 0x1B
#define DEFAULT_COLOUR DISP_FGND_WHITE + DISP_BGND_BLACK
#define wait(seconds)  OSTimeDlyHMSM(0, 0, seconds, 0)
#define waitMili(miliseconds)  OSTimeDlyHMSM(0, 0, 0, miliseconds)
#define tick(times) OSTimeDly(times)
#define print(x, y, msg) PC_DispStr(x, y, msg, DEFAULT_COLOUR)
#define EMPTY_STRING "                                                                    "
#include  "includes.h"

extern void errorHandler(char *str, UBYTE retnum, UBYTE returnOS){
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

extern void createTask(void* func, void* data, void* stack, byte prio){
    int status;
    status = OSTaskCreate(func, data, stack, prio);
    if(status != OS_ERR_NONE){
        errorHandler("ERROR: Error while creating a task:", status, 1);
    }
}

#endif /* COMMON_H */