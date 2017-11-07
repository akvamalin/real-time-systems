/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#define TASK_HIGH_PRIO 4
#define TASK_STACK_SIZE 512

#define INPUT_POS_Y 5
#define TASKS_POS_Y 6

#define ASCII_CODE_ZERO 49
#define ASCII_CODE_SPACE 32
#define TASKS_AMOUNT 10
#define ROUND_TRIPS 10000

#include "../common.h"

unsigned int ism = 0;

struct TaskData{
    int index;
    OS_EVENT *semaphore;
};

byte freePrio = TASK_HIGH_PRIO + 1;
OS_STK tasksStack[TASKS_AMOUNT][TASK_STACK_SIZE];
OS_STK initialTaskStack[TASK_STACK_SIZE];

int getFreePrio(){
    return freePrio++;
}

void simpleTask(void* data){    
    struct TaskData *tdata = (struct TaskData*)data;
    int *shmptr = &ism;
    int number = 0;
    int i = 0;
    for(i; i < ROUND_TRIPS; i++){
        number = *shmptr;
        number++;
        // here we simulate some sort of task switch
        if(tdata->index == 7){
            if(i == 5000){
                wait(1);
            }
        }
        *shmptr = number; 
    }
    // every task shoud be continiously executed
    // if the task exits, we become scheduling error
    while(1){
        wait(1);
    }
}

void initialTask(void* data){    
    char buffer[10];
    int i = 0;
    INT16S key;
    struct TaskData tasks[TASKS_AMOUNT];
    
    for (i = 0; i < TASKS_AMOUNT; i++) {
        tasks[i].index = i;
        tasks[i].semaphore = OSSemCreate(0);;
        createTask(simpleTask, (void *)&tasks[i], (void *)&tasksStack[i][TASK_STACK_SIZE - 1],getFreePrio());
    }

    while(1){
        if(PC_GetKey(&key)){
            if(key == KEY_ESC){
                exit(0);
            }
        }
        sprintf(buffer, "Number: %d", ism);
        print(0, INPUT_POS_Y, buffer);
        wait(1);
    }
}

int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    print(26,  0, "uC/OS-II, The Real-Time Kernel");
    print(33,  1, "Shared Memory");
    print(34,  2, "Yevhenii Maliavka");
    print(33,  3, "(press ESC to exit)");
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], freePrio++);
    OSStart();
    return 0;
}