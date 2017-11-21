/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#define TASK_HIGH_PRIO 4
#define TASK_STACK_SIZE 512

#define SYS_INFO_POS_Y 5
#define TASKS_POS_Y 6

#define ASCII_CODE_ZERO 49
#define ASCII_CODE_SPACE 32

#include "../common.h"

struct TaskData{
    int index;
    char* name;
    OS_EVENT *semaphore;
};

byte freePrio = TASK_HIGH_PRIO + 1;
struct TaskData parentTask, childTask;
OS_STK parentStck[TASK_STACK_SIZE], childStck[TASK_STACK_SIZE], initialTaskStack[TASK_STACK_SIZE];

int getFreePrio(){
    return freePrio++;
}

void parentTaskFunc(void* data){
    struct TaskData* tdata = (struct TaskData*)data;
    UBYTE err;
    char strCounter[1];
    int counter = 0;
    char info[] = "Parent task is blocked ";
    int tab = strlen(info);

    while(TRUE){
        OSSemPend(tdata->semaphore, OS_TICKS_PER_SEC, &err);
        if(err != 0){
            print(0, TASKS_POS_Y + 1, info);
            sprintf(strCounter, "%d", ++counter);
            print(tab , TASKS_POS_Y + 1, strCounter);
            continue;
        }
        counter = 0;
        print(0, TASKS_POS_Y + 1, "Parent task has entered!");
    }    
}

void childTaskFunc(void* data){
    struct TaskData* tdata = (struct TaskData*)data;
    while(TRUE){
        print(0, TASKS_POS_Y, "Child task: blocking parent!");
        wait(4);
        print(0, TASKS_POS_Y, "Child task: released parent!");
        OSSemPost(tdata->semaphore);
        wait(1);
    }    
}

void initialTask(void* data){    
    char buffer[100];
    int i = 0;
    INT16S key;

    parentTask.name = "Parent task";
    parentTask.semaphore = OSSemCreate(0);
    createTask(parentTaskFunc, (void *)&parentTask, (void *)&parentStck, getFreePrio());

    childTask.name = "Child task";
    childTask.semaphore = parentTask.semaphore;
    createTask(childTaskFunc, (void *)&childTask, (void *)&childStck, getFreePrio());

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
    print(26,  0, "uC/OS-II, The Real-Time Kernel");
    print(33,  1, "Priority Inversion");
    print(34,  2, "Yevhenii Maliavka");
    print(33,  3, "(press ESC to exit)");
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], freePrio++);
    OSStart();
    return 0;
}