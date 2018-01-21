/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#include "../common.h"

#define TASK_HIGH_PRIO 4
#define TASK_STACK_SIZE 512

OS_STK initialTaskStack[TASK_STACK_SIZE];

byte freePrio = TASK_HIGH_PRIO + 1;

int getFreePrio(){
    return freePrio++;
}

void initialTask(void* data){    
    char buffer[100];
    INT16S key;
    
    sprintf(buffer, "Initial task is running...");
    print(30, 5, buffer);
    while(1){
        if(PC_GetKey(&key)){
            if(key == KEY_ESC){
                exit(0);
            }
        }
        wait(1);
    }
}

int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    print(30,  0, "uC/OS-II, The Real-Time Kernel");
    print(32,  1, "Automated Machine Project");
    print(34,  2, "Yevhenii Maliavka");
    print(33,  3, "(press ESC to exit)");
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], getFreePrio());
    OSStart();
    return 0;
}