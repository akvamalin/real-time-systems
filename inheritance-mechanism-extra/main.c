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
#include "math.h"
#include "limits.h"

struct TaskData{
    int index;
    char* name;
    OS_EVENT *mutex;
};

byte freePrio = TASK_HIGH_PRIO + 1;

struct TaskData parentTask, childTask, middle1Task, middle2Task;
OS_STK parentStck[TASK_STACK_SIZE], 
       childStck[TASK_STACK_SIZE],
       middle1TaskStck[TASK_STACK_SIZE], 
       middle2TaskStck[TASK_STACK_SIZE], 
       initialTaskStack[TASK_STACK_SIZE];

int getFreePrio(){
    return freePrio++;
}

void CPULoad(int x, int y, int limit){
    int i = 0, j = 0, result = 0;
    char buf[100];
    for(i = 0; i < limit; i++){
        for(j; j < 10000; j++){
            sprintf(buf, "%d", j);
            print(x, y, buf);
        }
    }
}

void parentTaskFunc(void* data){
    struct TaskData* tdata = (struct TaskData*)data;
    UBYTE err;
    char strCounter[10], msg[100];
    int entered = 0;
    char info[] = "Parent task is blocked ";
    int tab = strlen(info);

    while(TRUE){
        sprintf(msg, "Already entered: %d", entered);
        print(0, TASKS_POS_Y, msg);
        OSMutexPend(tdata->mutex, 0, &err);
        if(err != 0){
            sprintf(msg, "%s", info, entered);
            print(0, TASKS_POS_Y, info);
            print(tab , TASKS_POS_Y, strCounter);
            continue;
        }
        entered++;
        print(0, TASKS_POS_Y, EMPTY_STRING);
    }    
}

void middleTaskFunc(void* data){
    struct TaskData *tdata = (struct TaskData*)data;
    char buf[100];
    int counter = 0;
    while(TRUE){
        sprintf(buf, "%s. Done %d times. CPU Load ", tdata->name, counter++);
        print(0, TASKS_POS_Y + tdata->index, buf);
        CPULoad(strlen(buf) + 1,TASKS_POS_Y + tdata->index, 1);
        wait(1);
    }    
}

void childTaskFunc(void* data){
    struct TaskData* tdata = (struct TaskData*)data;
    char buffer[256];
    char msg[] = "Child task: blocking parent! CPU Load";
    while(TRUE){
        print(0, TASKS_POS_Y + 3, EMPTY_STRING);
        print(0, TASKS_POS_Y + 3, msg);
        CPULoad(strlen(msg) + 1, TASKS_POS_Y + 3, 1);
        print(0, TASKS_POS_Y + 3, "Child task: CPU load done!Released parent!");
        wait(1);
        OSMutexPost(tdata->mutex);
    }    
}

void initialTask(void* data){    
    char buffer[100];
    int i = 0;
    INT16S key;
    UBYTE err;

    int mutexPrio = getFreePrio();

    parentTask.name = "Parent task";
    parentTask.mutex = OSMutexCreate(mutexPrio, &err);
    OSMutexPend(parentTask.mutex, 0, &err);
    createTask(parentTaskFunc, (void *)&parentTask, (void *)&parentStck, getFreePrio());

    middle1Task.name = "Middle 1 task started";
    middle1Task.index = 1;
    createTask(middleTaskFunc, (void *)&middle1Task, (void *)&middle1TaskStck, getFreePrio());

    middle2Task.name = "Middle 2 task started";
    middle2Task.index = 2;
    createTask(middleTaskFunc, (void *)&middle2Task, (void *)&middle2TaskStck, getFreePrio());

    childTask.name = "Child task";
    childTask.mutex = parentTask.mutex;
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
    print(33,  1, "Inheritance Mechanism");
    print(34,  2, "Yevhenii Maliavka");
    print(33,  3, "(press ESC to exit)");
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], getFreePrio());
    OSStart();
    return 0;
}