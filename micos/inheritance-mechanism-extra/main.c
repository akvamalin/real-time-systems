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

void CPULoad(int limit){
    int i = 0, j = 0, result = 0;
        for(j; j < 100000 * limit; j++){
            result += sin(i) * cos(j) * tan(j) + (sin(j) * cos(i) * (tan(i))*sin(j) * cos(i) * tan(i));
        }
}

void parentTaskFunc(void* data){
    struct TaskData* tdata = (struct TaskData*)data;
    UBYTE err;
    char strCounter[10], msg[100];
    int entered = 0, timeout = 0;
    char info[] = "Parent task is blocked ";
    int tab = strlen(info);

    while(TRUE){
        memset(&msg[0], 0, sizeof(msg));
        OSMutexPend(tdata->mutex, 0, &err);
        if(err != 0){
            timeout++;
            sprintf(msg, "%s, entered: %d, timeout: %d", tdata->name, entered, timeout);
            print(0, TASKS_POS_Y, msg);
            print(tab , TASKS_POS_Y, strCounter);
            continue;
        }
        entered++;

        print(0, TASKS_POS_Y, EMPTY_STRING);
        sprintf(msg, "%s, entered: %d, timeout: %d", tdata->name, entered, timeout);
        print(0, TASKS_POS_Y, msg);
        
        OSMutexPost(tdata->mutex);
        wait(1);
    }    
}

void middleTaskFunc(void* data){
    struct TaskData *tdata = (struct TaskData*)data;
    char buf[100];
    int counter = 0;
    while(TRUE){
        sprintf(buf, "%s. Done %d times. CPU Load ", tdata->name, counter++);
        print(0, TASKS_POS_Y + tdata->index, buf);
        CPULoad(10);
        sprintf(buf, "%s. Done %d times. ", tdata->name, counter++);
        print(0, TASKS_POS_Y + tdata->index, buf);
        wait(1);
    }    
}

void childTaskFunc(void* data){
    struct TaskData* tdata = (struct TaskData*)data;
    UBYTE err;
    char msg[] = "Child task: blocking parent! CPU Load...";
    char buf[100];
    int released = 0;
    while(TRUE){
        OSMutexPend(tdata->mutex, 0, &err);
        print(0, TASKS_POS_Y + 3, EMPTY_STRING);
        sprintf(buf, "%s released key: %d", msg, released);
        print(0, TASKS_POS_Y + 3, buf);
        memset(&buf[0], 0, sizeof(buf));
        CPULoad(100);
        OSMutexPost(tdata->mutex);
        released++;
    }    
}

void initialTask(void* data){    
    char buffer[100];
    int i = 0;
    INT16S key;
    UBYTE err;

    
    int freeHighestPrio = getFreePrio();
    parentTask.name = "Parent task";
    parentTask.mutex = OSMutexCreate(freeHighestPrio, &err);
    createTask(parentTaskFunc, (void *)&parentTask, (void *)&parentStck, getFreePrio());

    middle1Task.name = "Middle 1 task";
    middle1Task.index = 1;
    createTask(middleTaskFunc, (void *)&middle1Task, (void *)&middle1TaskStck, getFreePrio());

    middle2Task.name = "Middle 2 task";
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