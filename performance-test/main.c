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

#include "../common.h"

struct TaskData{
    int index;
    OS_EVENT *semaphore;
};

struct TaskData *tasks;
int roundTrips = 0, tasksAmount = 0;
byte freePrio = TASK_HIGH_PRIO + 1;
OS_STK **tasksStack;
OS_STK initialTaskStack[TASK_STACK_SIZE];

int getFreePrio(){
    return freePrio++;
}

void simpleTask(void* data){    
    struct TaskData* tdata = (struct TaskData*)data;
    int tab;
    char buffer[100];
    UBYTE err;
    sprintf(buffer, "%d. Task ", tdata->index);
    tab = strlen(buffer);
    print(0, TASKS_POS_Y + tdata->index, buffer);
    
    while(TRUE){
        print(tab, TASKS_POS_Y + tdata->index, "pending...");
        OSSemPend(tdata->semaphore, 0, &err);
        print(tab, TASKS_POS_Y + tdata->index, "active!");
        if(tdata->index + 1 >= tasksAmount){
            roundTrips++;
            OSSemPost(tasks[0].semaphore);
        }
        else{
            OSSemPost(tasks[tdata->index + 1].semaphore);
        }
    }
}

void initialTask(void* data){    
    char buffer[100];
    int i = 0;
    INT16S key;
    UBYTE started = 0;

    // Allocate memory for tasks and stack dynamically
    print(0, INPUT_POS_Y, "Enter a number of tasks to create: ");
    scanf("%d", &tasksAmount);
    tasks = malloc(tasksAmount * sizeof(struct TaskData));
    tasksStack = malloc(tasksAmount * sizeof(OS_EVENT*));
    for(i = 0; i < tasksAmount; i++){
        tasksStack[i] = malloc(sizeof(OS_EVENT));
    }

    // Initiate and create tasks
    for (i = 0; i < tasksAmount; i++) {
        tasks[i].index = i;
        tasks[i].semaphore = OSSemCreate(0);;
        createTask(simpleTask, (void *)&tasks[i], (void *)&tasksStack[i][TASK_STACK_SIZE - 1],getFreePrio());
    }

    while(1){
        if(PC_GetKey(&key)){
            if(key == KEY_ESC){
                exit(0);
            }
    
            if(!started && key == ASCII_CODE_SPACE){
                print(0, INPUT_POS_Y, "Started! The first semaphore is released");
                started = 1;
                OSSemPost(tasks[0].semaphore);
            }
        }
        sprintf(buffer, "Roundtrips: %d", roundTrips);
        print(0, TASKS_POS_Y + tasksAmount, buffer);
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