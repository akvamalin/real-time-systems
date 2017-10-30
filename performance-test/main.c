/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#define TASK_HIGH_PRIO 4
#define TASK_STACK_SIZE 512
#define N_TASKS_AMOUNT 21
#define INPUT_POS_Y 5
#define TASKS_POS_Y 6
#define ASCII_CODE_ZERO 49
#define ASCII_CODE_SPACE 32
#include "../common.h"

struct TaskData{
    int index;
    OS_EVENT *semaphore;
};

struct TaskData tasks[N_TASKS_AMOUNT];
int roundTrips = 0;
byte freePrio = TASK_HIGH_PRIO + 1;
OS_STK tasksStack[N_TASKS_AMOUNT][TASK_STACK_SIZE];
OS_STK initialTaskStack[TASK_STACK_SIZE];

int getFreePrio(){
    return freePrio++;
}

void initStruct(struct TaskData* data, int index){
    data->index = index;
    data->semaphore = OSSemCreate(0);
}

void simpleTask(void* data){    
    struct TaskData* tdata = (struct TaskData*)data;
    char buffer[100];
    UBYTE err;
    sprintf(buffer, "%d. Task ", tdata->index);
    int tab = strlen(buffer);
    print(0, tdata->index, buffer);
    OSSemPend(tdata->semaphore, 0, &err);
    
    while(TRUE){
        print(tab, tdata->index, "active!");
        if(tdata->index + 1 >= N_TASKS_AMOUNT){
            roundTrips++;
            OSSemPost(tasks[0].semaphore);
        }
        else{
            OSSemPost(tasks[tdata->index + 1].semaphore);
        }
        print(tab, tdata->index, "pending...");
        OSSemPend(tdata->semaphore, 0, &err);
        // tick(1);
    }
}

void initialTask(void* data){    
    char buffer[100];
    int i = 0;
    INT16S key;
    UBYTE started = 0;

    for (i = 0; i < N_TASKS_AMOUNT; i++) {
        initStruct(&tasks[i], i);
        createTask(simpleTask, (void *)&tasks[i], (void *)&tasksStack[i][TASK_STACK_SIZE - 1],getFreePrio());
    }

    while(1){
        if(PC_GetKey(&key)){
            if(key == KEY_ESC){
                exit(0);
            }
    
            if(!started && key == ASCII_CODE_SPACE){
                print(0, 12, "Started! The first semaphore is released");
                started = 1;
                OSSemPost(tasks[0].semaphore);
            }
        }
        sprintf(buffer, "Roundtrips: %d", roundTrips);
        print(0, INPUT_POS_Y + 20, buffer);
        roundTrips = 0;
        wait(1);
    }
}

int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    print(26,  0, "uC/OS-II, The Real-Time Kernel");
    print(33,  1, "Performance Test");
    print(34,  2, "Yevhenii Maliavka");
    print(33,  3, "(press ESC to exit)");
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], freePrio++);
    OSStart();
    return 0;
}