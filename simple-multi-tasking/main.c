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

OS_STK tasksStack[TASKS_AMOUNT][TASK_STACK_SIZE];
OS_STK initialTaskStack[TASK_STACK_SIZE];
OS_EVENT* randomSemaphore;

void initialTask(void* data){
    while(1){
        PC_DispStr(0, 0, "Hello, World", DISP_FGND_BLUE);
    }
}

int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    OSTaskCreate(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], TASK_HIGH_PRIO + 1);
    OSStart();
    return 0;
}