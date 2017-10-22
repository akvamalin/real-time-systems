/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#define TASK_HIGH_PRIO 4
#define TASK_STACK_SIZE 512
#define N_TASKS_AMOUNT 4
#define INPUT_POS_Y 5
#define TASKS_POS_Y 6
#include "../common.h"

int tasksData[N_TASKS_AMOUNT];
byte freePrio = TASK_HIGH_PRIO + 1;
OS_STK tasksStack[N_TASKS_AMOUNT][TASK_STACK_SIZE];
OS_STK initialTaskStack[TASK_STACK_SIZE];

int getFreePrio(){
    return freePrio++;
}

struct TaskData{
    int cCounter;
    int tCounter;
    int id;
    time_t lastCall;
    OS_EVENT *semaphore;
};

void simpleTask(void* data){    
    struct TaskData* tdata = (struct TaskData*)data;
    char buffer[100];
    int prevCall = -1;
    UBYTE err;
    
    while(TRUE){
        OSSemPend(tdata->semaphore, 0, &err);
        print(0, TASKS_POS_Y + tdata->id, EMPTY_STRING);
        sprintf(buffer, "id: %d |cc: %d | tc: %d", tdata->id, tdata->cCounter, tdata->tCounter);
        print(0, TASKS_POS_Y + tdata->id, buffer);
        prevCall = tdata->cCounter;
        OSSemPost(tdata->semaphore);
        tick(1);
    }
}

void initialTask(void* data){    
    struct TaskData tasks[N_TASKS_AMOUNT];
    char keyBuffer[100];
    char msg[] = "Calling task:";
    int tab = 0, i = 0, keyNumber = 0;
    double timeSpent = 0.0;
    INT16S key;
    UBYTE err;

    for (i = 0; i < N_TASKS_AMOUNT; i++) {
        tasks[i].id = i + 1;
        tasks[i].cCounter = 0;
        tasks[i].tCounter = 0;
        time(&tasks[i].lastCall);
        createTask(simpleTask, (void *)&tasks[i], (void *)&tasksStack[i][TASK_STACK_SIZE - 1],getFreePrio());
    }
    i = 0;

    print(0, INPUT_POS_Y, msg);
    time_t endTime;
    while(1){
        for(i = 0; i < N_TASKS_AMOUNT; i++){
            time(&endTime);
            timeSpent = difftime(endTime, tasks[i].lastCall); 
            char buf1[100];
            sprintf(buf1, "Stopwatch: %.1f", timeSpent);
            print(0, 2, buf1);
            if( timeSpent >= 4.0){
                OSSemPend(tasks[0].semaphore, 0, &err);
                tasks[i].tCounter++;
                time(&tasks[i].lastCall);
                OSSemPost(tasks[0].semaphore);
            }
        }
        if(PC_GetKey(&key)){
            if(key == KEY_ESC){
                exit(0);
            }
            tab = strlen(msg);
            print(tab, INPUT_POS_Y, EMPTY_STRING);
            sprintf(keyBuffer, "%c", key);
            switch(key){
                case 49: 
                    time(&tasks[0].lastCall);
                    print(tab, INPUT_POS_Y, keyBuffer);
                    OSSemPend(tasks[0].semaphore, 0, &err);
                    tasks[0].cCounter++;
                    OSSemPost(tasks[0].semaphore);
                break;
                default:
                    sprintf(keyBuffer, "%c is undefined.", key);
                    print(tab, INPUT_POS_Y, keyBuffer);
                break;
            }
        }
        waitMili(200);
    }
}

int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    print(26,  0, "uC/OS-II, The Real-Time Kernel");
    print(33,  1, "Inter-process communication mechanisms");
    print(34,  2, "Yevhenii Maliavka");
    print(33,  3, "(press ESC to exit)");
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], freePrio++);
    OSStart();
    return 0;
}