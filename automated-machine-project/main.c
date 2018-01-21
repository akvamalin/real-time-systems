/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#include "../common.h"

#define MAX_BUFFER_SIZE 100
#define TASK_HIGH_PRIO 4
#define TASK_STACK_SIZE 512

#define POS_START_Y 5
#define POS_START_X 0

#define POS_COMPONENT_A_Y 6


#define WEIHGING_MACHINES_COUNT 2

OS_STK initialTaskStack[TASK_STACK_SIZE], 
       weighingMachine1Stack[TASK_STACK_SIZE],
       UITaskStack[TASK_STACK_SIZE];

byte freePrio = TASK_HIGH_PRIO + 1;

struct WeighingMachine{
    int id;
    int capacity;
    int componentA;
    int componentB;
    int componentC;
    OS_EVENT *semaphore;
};

byte getFreePrio(){
    return freePrio++;
}

struct WeighingMachine machines[WEIHGING_MACHINES_COUNT];

void fillComponentATask(void* data){
    UBYTE err;
    int seconds = 0;
    char buffer[MAX_BUFFER_SIZE];
    char *taskName = "[Component A Task]";
    char *statusWait = "Awaiting";
    char *statusProgress = "Filling";
    struct WeighingMachine* machine = (struct WeighingMachine*)data;
    while(1){
        memset(buffer, 0, MAX_BUFFER_SIZE);
        print(POS_START_X, POS_COMPONENT_A_Y, buffer);
        // add this 40 as recipe
        if(machine->componentA >= 40){
            sprintf(buffer, "%s Machine %d is full with component A. %s...", taskName, machine->id, statusWait);
            print(POS_START_X, POS_COMPONENT_A_Y, buffer);
            wait(1);
            continue;
        }
        sprintf(buffer, "%s %s %d secs...", taskName, statusWait, seconds);
        print(POS_START_X, POS_COMPONENT_A_Y, buffer);
        OSSemPend(machine->semaphore, OS_TICKS_PER_SEC, &err);
        if(err != 0){
            seconds++;
            continue;
        }
        memset(buffer, 0, MAX_BUFFER_SIZE);
        print(POS_START_X, POS_COMPONENT_A_Y, EMPTY_STRING);
        sprintf(buffer, "%s %d %s...", taskName, machine->id, statusProgress);
        print(POS_START_X, POS_COMPONENT_A_Y, buffer);
        machine->componentA += machine->capacity;
        // Add this wait as config
        wait(1);
        seconds = 0;
        OSSemPost(machine->semaphore);
    }
}

void UITask(){
    UBYTE err;
    char buffer[MAX_BUFFER_SIZE];
    while(1){
        OSSemPend(machines[0].semaphore, 0, &err);
        if(err){
            print(0,0, "UNKONW ERROR OCCURED IN UITASK");
            continue;
        }
        memset(buffer, 0, 100);
        print(POS_START_X, POS_START_Y, buffer);
        sprintf(buffer, "[Machine 1] componentA %d", machines[0].componentA);
        print(POS_START_X, POS_START_Y, buffer);
        OSSemPost(machines[0].semaphore);
        wait(1);
    }
}

void initialTask(void* data){    
    char buffer[MAX_BUFFER_SIZE];
    INT16S key;
    int iterator = 0;

    // Initiate weighing machines
    for(iterator; iterator < WEIHGING_MACHINES_COUNT; iterator++){
        machines[iterator].id = iterator;
        machines[iterator].componentA = machines[iterator].componentB = machines[iterator].componentC = 0;
        machines[iterator].capacity = 10;
        machines[iterator].semaphore = OSSemCreate(1);
    }

    createTask(fillComponentATask, &machines[0], weighingMachine1Stack, getFreePrio());
    createTask(UITask, NULL, UITaskStack, getFreePrio());

    sprintf(buffer, "Initial task is running!");
    print(POS_START_X, POS_START_Y, buffer);
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