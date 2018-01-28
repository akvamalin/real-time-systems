/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

// INCLUDES
///////////////////////////////////////////////////////////////////////////////////
#include "../common.h"
#include "scales.h"
#include "mixer.h"
#include "watch.h"
#include "recipe.h"


// DEFINITIONS
///////////////////////////////////////////////////////////////////////////////////
#define MAX_BUFFER_SIZE 256
#define TASK_STACK_SIZE 512

// INITIAL TASK COORDINATES
#define POS_X_INITIAL_TASK 35
#define POS_Y_INITIAL_TASK 2
#define COMPONENTS_COUNT 6
#define RECIPE_PATH "recipe.txt"

// FUNCTIONS DECLARATIONS
///////////////////////////////////////////////////////////////////////////////////
void initialTask(void* data);
int calcTotalLoad(struct Recipe* recipe);

// GLOBAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////
OS_STK  initialTaskStack[TASK_STACK_SIZE], 
        watchTaskStack[TASK_STACK_SIZE], 
        fillScaleComponentA1TaskStack[TASK_STACK_SIZE],
        fillScaleComponentB1TaskStack[TASK_STACK_SIZE],
        fillScaleComponentC1TaskStack[TASK_STACK_SIZE],
        fillScaleComponentA2TaskStack[TASK_STACK_SIZE],
        fillScaleComponentB2TaskStack[TASK_STACK_SIZE],
        fillScaleComponentC2TaskStack[TASK_STACK_SIZE],
        fillMixerTask1Stack[TASK_STACK_SIZE],
        fillMixerTask2Stack[TASK_STACK_SIZE],
        mixingDryTaskStack[TASK_STACK_SIZE],
        mixingWetTaskStack[TASK_STACK_SIZE];        

// MAIN ENTRY POINT
///////////////////////////////////////////////////////////////////////////////////
int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    print(30,  0, "uC/OS-II, The Real-Time Kernel / Yevhenii Maliavka");
    print(32,  1, "Automated Machine Project (press ESC to exit)");
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
    OSStart();
    return 0;
}

// FUNCTIONS DEFINITIONS
///////////////////////////////////////////////////////////////////////////////////
void initialTask(void* data){    
    INT16S key;
    printy(POS_X_INITIAL_TASK, POS_Y_INITIAL_TASK, "[INITIAL TASK]: working...");
    
    // OMG
    // Sorry for this initialization code...
    // I blame myself :(
    struct Mixer* mixer = (struct Mixer*)malloc(sizeof(struct Mixer));
    mixer->load = 0;
    mixer->semaphore = OSSemCreate(1);
    mixer->infP.x = 30;
    mixer->infP.y = 12;

    struct Recipe* recipe = NULL;
    struct Scale scale1, scale2;
    scale1.id = 1;
    scale1.semaphore = OSSemCreate(1);
    scale1.components = (struct Components*)malloc(sizeof(struct Components));
    scale1.components->A = scale1.components->B = scale1.components->C = 0;
    scale1.infP = (struct Point*)malloc(sizeof(struct Point));
    scale1.infP->x = 0;
    scale1.infP->y = 9;
    
    scale2.id = 2;
    scale2.semaphore = OSSemCreate(1);
    scale2.components = (struct Components*)malloc(sizeof(struct Components));
    scale2.components->A = scale2.components->B = scale2.components->C = 0;
    scale2.infP = (struct Point*)malloc(sizeof(struct Point));
    scale2.infP->x = 50;
    scale2.infP->y = 9;

    struct FillScaleTaskOpts a1, b1, c1;
    a1.componentName = 'A';
    a1.infP.x = 0;
    a1.infP.y = 4;
    a1.scale = &scale1;
    a1.status = 0;
    b1.componentName = 'B';
    b1.infP.x = 18;
    b1.infP.y = 4;
    b1.status = 0;
    b1.scale = &scale1;
    c1.componentName = 'C';
    c1.status = 0;
    c1.scale = &scale1;
    c1.infP.x = 33;
    c1.infP.y = 4;

    struct FillScaleTaskOpts a2, b2, c2;
    a2.componentName = 'A';
    a2.status = 0;
    a2.scale = &scale2;
    a2.infP.x = 48;
    a2.infP.y = 4;
    b2.componentName = 'B';
    b2.status = 0;
    b2.scale = &scale2;
    b2.infP.x = 63;
    b2.infP.y = 4;
    c2.componentName = 'C';
    c2.status = 0;
    c2.scale = &scale2;
    c2.infP.x = 78;
    c2.infP.y = 4;

    struct WatchTaskOpts wopts;
    struct FillScaleTaskOpts* allOpts[COMPONENTS_COUNT] = {&a1, &b1, &c1, &a2, &b2, &c2};
    wopts.scale1 = &scale1;
    wopts.scale2 = &scale2;
    wopts.fillingOpts = allOpts;
    wopts.mixer = mixer;
    wopts.componentsCount = COMPONENTS_COUNT;

    struct FillMixerTaskOpts fillMixerOpts1, fillMixerOpts2;
    fillMixerOpts1.mixer = mixer;
    fillMixerOpts1.scale = &scale1;
    
    fillMixerOpts2.mixer = mixer;
    fillMixerOpts2.scale = &scale2;

    struct MixingTaskOpts optsDry, optsWet;
    optsDry.mixer = mixer;
    optsDry.mixingDuration = 4;
    optsDry.infP.x = 0;
    optsDry.infP.y = 15;
    optsDry.mixingType = "dry";
    while(1){
        if(PC_GetKey(&key)){
            if(key == KEY_ESC){
                status("Key ESC pressed");
                exit(0);
            }
            if(key == KEY_SPACE){
                status("Key SPACE pressed");
                // Read recipe from a file
                ///////////////////////////////////////////////////////////////////////////////////
                status("Read recipe...");
                recipe = readRecipe(RECIPE_PATH);
                wopts.recipe = recipe;
                status("Recipe has been read");
                status("Start fillScaleComponentTask for scale 1");

                // set components volume limit
                ///////////////////////////////////////////////////////////////////////////////////
                a1.componentLimit = recipe->weight1->A;
                b1.componentLimit = recipe->weight1->B;
                c1.componentLimit = recipe->weight1->C;
                a2.componentLimit = recipe->weight2->A;
                b2.componentLimit = recipe->weight2->B;
                c2.componentLimit = recipe->weight2->C;
                fillMixerOpts1.limits = recipe->weight1;
                fillMixerOpts2.limits = recipe->weight2;
                mixer->loadLimit = calcTotalLoad(recipe);
                // Start watching task
                ///////////////////////////////////////////////////////////////////////////////////
                createTask(watchTask, (void*)&wopts, &watchTaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());

                // Start fillin components tasks
                ///////////////////////////////////////////////////////////////////////////////////
                createTask(mixingTask, (void*)&optsDry, &mixingDryTaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(fillMixerTask, (void*)&fillMixerOpts1, &fillMixerTask1Stack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(fillMixerTask, (void*)&fillMixerOpts2, &fillMixerTask2Stack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(fillScaleComponentTask, (void*)&a1, &fillScaleComponentA1TaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(fillScaleComponentTask, (void*)&b1, &fillScaleComponentB1TaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(fillScaleComponentTask, (void*)&c1, &fillScaleComponentC1TaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(fillScaleComponentTask, (void*)&a2, &fillScaleComponentA2TaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(fillScaleComponentTask, (void*)&b2, &fillScaleComponentB2TaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(fillScaleComponentTask, (void*)&c2, &fillScaleComponentC2TaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
            }
        }
        wait(1);
    }
}

int calcTotalLoad(struct Recipe* recipe){
    int totalWeight1 = recipe->weight1->A + recipe->weight1->B + recipe->weight1->C;
    int totalWeight2 = recipe->weight2->A + recipe->weight2->B + recipe->weight2->C;
    return totalWeight1 + totalWeight2;
}