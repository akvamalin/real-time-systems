/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#include  "includes.h"
#define UBYTE INT8U
#define TASK_HIGH_PRIO 4
#define TASK_STACK_SIZE 512
#define TASKS_AMOUNT 10
#define KEY_ESC 0x1B

#define BUFFER 100
#define CLOCK_POS_X 0
#define CLOCK_POS_Y 10
#define STATS_POS_X 0
#define STATS_POS_Y 11

#define EMPTY_STRING "\r"
#define DEFAULT_COLOUR DISP_FGND_WHITE + DISP_BGND_BLACK
#define INT_SIZE 16

#define wait(seconds)  OSTimeDlyHMSM(0, 0, seconds, 0)
#define tick(times) OSTimeDly(times)
#define print(x, y, msg) PC_DispStr(x, y, msg, DEFAULT_COLOUR)

OS_STK tasksStack[TASKS_AMOUNT][TASK_STACK_SIZE];
OS_STK initialTaskStack[TASK_STACK_SIZE];
byte freePrio = TASK_HIGH_PRIO + 1;

void clockTask(void* data){
    char s[BUFFER];
    UBYTE err;
    print(CLOCK_POS_X, CLOCK_POS_Y, (char*) data);
    int tab = strlen((char*)data);
    while(TRUE){
        PC_GetDateTime(s);
        print(CLOCK_POS_X + tab, CLOCK_POS_Y, EMPTY_STRING);
        print(CLOCK_POS_X + tab, CLOCK_POS_Y, s);
        wait(1);
    }
}

void statsTask(void* data){
    char s[BUFFER];
    print(STATS_POS_X, STATS_POS_Y, (char*) data);
    int tab = strlen((char*)data);
    OSStatInit();
    while(TRUE){
        sprintf(s, "%d tasks, %d CPU, %d switches", OSTaskCtr, OSCPUUsage, OSCtxSwCtr);
        print(STATS_POS_X + tab, STATS_POS_Y, s);
        OSCtxSwCtr = 0;
        wait(1);
    }
}

void initialTask(void* data){
    print(26,  0, "uC/OS-II, The Real-Time Kernel");
    print(33,  1, "Simple multi-tasking");
    print(34,  2, "Yevhenii Maliavka");
    print(33,  3, "(press ESC to exit)");
    
    char keyBuffer[INT_SIZE];
    char msg[] = "Last key pressed: ";
    int x = 0, y = 12;
    int normalKey;
    int tab;
    INT16S key;

    OSTaskCreate(clockTask, (void*)"Time:", (void*)&tasksStack[0][TASK_STACK_SIZE - 1], freePrio++);
    OSTaskCreate(statsTask, (void*)"Statistics:", (void*)&tasksStack[1][TASK_STACK_SIZE - 1], freePrio++);

    print(x, y, msg);
    while(1){
        if(PC_GetKey(&key)){
            if(key == KEY_ESC){
                exit(0);
            }
            tab = strlen(msg);
            sprintf(keyBuffer, "%d", key);
            sprintf(keyBuffer, "%c", (int)strtol(keyBuffer, NULL, 10));
            print(x + tab, y, EMPTY_STRING);
            print(x + tab, y, keyBuffer);
        }
        tick(1);
    }
}

int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    OSTaskCreate(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], freePrio++);
    OSStart();
    return 0;
}