/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

// INCLUDES
///////////////////////////////////////////////////////////////////////////////////
#include "../common.h"


// DEFINITIONS
///////////////////////////////////////////////////////////////////////////////////
#define MAX_BUFFER_SIZE 256
#define TASK_STACK_SIZE 512


// INITIAL TASK COORDINATES
#define POS_X_INITIAL_TASK 35
#define POS_Y_INITIAL_TASK 6
#define COMPONENTS_COUNT 6
#define RECIPE_PATH "recipe.txt"


// STRUCTURES
///////////////////////////////////////////////////////////////////////////////////
struct Components{
    int A;
    int B;
    int C;
};

struct Point{
    int x;
    int y;
};

struct Recipe{
    struct Components* weight1;
    struct Components* weight2;
    int waterDuration;
    int drymixDuration;
    int wetmixDuration;
};

struct Scale{
    int id;
    OS_EVENT* semaphore;
    struct Components* components;
    struct Point* infP;
};

struct FillScaleTaskOpts{
    char componentName;
    struct Scale* scale;
    int componentLimit;
    struct Point infP;
    int status;
};

struct Mixer{
    int load;
    int status;
    OS_EVENT* semaphore;
    struct Point infP;
};

struct FillMixerTaskOpts{
    struct Scale* scale;
    struct Mixer* mixer;
    struct Components* limits;
    int totalLoad;
};

struct WatchTaskOpts{
    struct Scale* scale1;
    struct Scale* scale2;
    struct Recipe* recipe;
    struct FillScaleTaskOpts** fillingOpts;
    struct Mixer* mixer;
};


// FUNCTIONS DECLARATIONS
///////////////////////////////////////////////////////////////////////////////////
struct Recipe* readRecipe(char* path);
void initialTask(void* data);
void fillScaleComponentTask(void* data);
void watchTask(void* data);
void fillScaleComponent(const struct FillScaleTaskOpts* opts);
byte isScaleComponentFull(const struct FillScaleTaskOpts* opts);
void fill(int* a, const int* b);
void displayScaleInfo(const struct Scale* scale, const struct Components* components);
void displayFillingComponentInfo(const struct FillScaleTaskOpts* opts);
void fillMixerTask(void* data);
void displayMixer(const struct Mixer* mixer, int total);
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
        fillMixerTask2Stack[TASK_STACK_SIZE];


// MAIN
// ENTRY POINT
///////////////////////////////////////////////////////////////////////////////////
int main(void){
    setbuf(stdout, NULL);
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_GRAY);
    OSInit();
    print(30,  1, "uC/OS-II, The Real-Time Kernel");
    print(32,  2, "Automated Machine Project");
    print(34,  3, "Yevhenii Maliavka");
    print(33,  4, "(press ESC to exit)");
    createTask(initialTask, (void*)0, &initialTaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
    OSStart();
    return 0;
}


// FUNCTIONS DEFINITIONS
///////////////////////////////////////////////////////////////////////////////////
void initialTask(void* data){    
    INT16S key;
    printy(POS_X_INITIAL_TASK, POS_Y_INITIAL_TASK, "[INITIAL TASK]: working...");
    
    struct Mixer* mixer = (struct Mixer*)malloc(sizeof(struct Mixer));
    mixer->load = 0;
    mixer->status = 0;
    mixer->semaphore = OSSemCreate(1);
    mixer->infP.x = 30;
    mixer->infP.y = 20;

    struct Recipe* recipe = NULL;
    struct Scale scale1, scale2;
    scale1.id = 1;
    scale1.semaphore = OSSemCreate(1);
    scale1.components = (struct Components*)malloc(sizeof(struct Components));
    scale1.components->A = scale1.components->B = scale1.components->C = 0;
    scale1.infP = (struct Point*)malloc(sizeof(struct Point));
    scale1.infP->x = 0;
    scale1.infP->y = 15;
    
    scale2.id = 2;
    scale2.semaphore = OSSemCreate(1);
    scale2.components = (struct Components*)malloc(sizeof(struct Components));
    scale2.components->A = scale2.components->B = scale2.components->C = 0;
    scale2.infP = (struct Point*)malloc(sizeof(struct Point));
    scale2.infP->x = 50;
    scale2.infP->y = 15;

    struct FillScaleTaskOpts a1, b1, c1;
    a1.componentName = 'A';
    a1.infP.x = 0;
    a1.infP.y = 10;
    a1.scale = &scale1;
    a1.status = 0;
    b1.componentName = 'B';
    b1.infP.x = 18;
    b1.infP.y = 10;
    b1.status = 0;
    b1.scale = &scale1;
    c1.componentName = 'C';
    c1.status = 0;
    c1.scale = &scale1;
    c1.infP.x = 33;
    c1.infP.y = 10;

    struct FillScaleTaskOpts a2, b2, c2;
    a2.componentName = 'A';
    a2.status = 0;
    a2.scale = &scale2;
    a2.infP.x = 48;
    a2.infP.y = 10;
    b2.componentName = 'B';
    b2.status = 0;
    b2.scale = &scale2;
    b2.infP.x = 63;
    b2.infP.y = 10;
    c2.componentName = 'C';
    c2.status = 0;
    c2.scale = &scale2;
    c2.infP.x = 78;
    c2.infP.y = 10;

    struct WatchTaskOpts wopts;
    struct FillScaleTaskOpts* allOpts[COMPONENTS_COUNT] = {&a1, &b1, &c1, &a2, &b2, &c2};
    wopts.scale1 = &scale1;
    wopts.scale2 = &scale2;
    wopts.fillingOpts = allOpts;
    wopts.mixer = mixer;

    struct FillMixerTaskOpts fillMixerOpts1, fillMixerOpts2;
    fillMixerOpts1.mixer = mixer;
    fillMixerOpts1.scale = &scale1;
    
    fillMixerOpts2.mixer = mixer;
    fillMixerOpts2.scale = &scale2;

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

                // Initialize filling components tasks
                ///////////////////////////////////////////////////////////////////////////////////

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
                fillMixerOpts1.totalLoad = fillMixerOpts2.totalLoad = calcTotalLoad(recipe);
                

                // Start watching task
                ///////////////////////////////////////////////////////////////////////////////////
                createTask(watchTask, (void*)&wopts, &watchTaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());

                // Start fillin components tasks
                ///////////////////////////////////////////////////////////////////////////////////
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

void unloadComponent(int* source, int* destination){
    int loadCapacity = 10;
    while(*source){
        *destination += loadCapacity;
        *source -= loadCapacity;
        wait(1);
    }
}

byte isScaleFull(const struct Components* values, const struct Components* limits){
    if(values->A != limits->A || values->B != limits->B || values->C != limits->C){
        return 0;
    }
    return 1;
}

void fillMixerTask(void* data){
    UBYTE err;
    struct FillMixerTaskOpts* opts = (struct FillMixerTaskOpts*)data;
    while(1){
        OSSemPend(opts->scale->semaphore, 0, &err);
        if(err){
            printy(0, 1, "A UNKNOWN ERROR in MIXER! Terminating");
            wait(5);
            exit(1);
        }
        // WTF
        if(!isScaleFull(opts->scale->components, opts->limits)){
            OSSemPost(opts->scale->semaphore);
            wait(1);
            continue;
        }
        OSSemPend(opts->mixer->semaphore, 0, &err);
        opts->mixer->status = 0;
        if(err){
            printy(0, 1, "B UNKNOWN ERROR in MIXER! Terminating");
            wait(1);
            exit(1);
        }
        if(opts->mixer->load >= opts->totalLoad){
            wait(1);
            continue;
        }
        opts->mixer->status = 1;
        unloadComponent(&(opts->scale->components->A), &(opts->mixer->load));
        unloadComponent(&(opts->scale->components->B), &(opts->mixer->load));
        unloadComponent(&(opts->scale->components->C), &(opts->mixer->load));
        OSSemPost(opts->mixer->semaphore);
        OSSemPost(opts->scale->semaphore);
        wait(1);
    }
}

void watchTask(void* data){
    UBYTE err;
    struct WatchTaskOpts* opts = (struct WatchTaskOpts*)data;
    struct FillScaleTaskOpts** headFillingOpts = opts->fillingOpts;
    int i;
    int totalLoad = calcTotalLoad(opts->recipe);
    while(1){
        displayScaleInfo(opts->scale1, opts->recipe->weight1);
        displayScaleInfo(opts->scale2, opts->recipe->weight2);
        for(i = 0; i < COMPONENTS_COUNT; i++){
            displayFillingComponentInfo(*(opts->fillingOpts)++);
        }
        opts->fillingOpts = headFillingOpts;
        displayMixer(opts->mixer, totalLoad);
        wait(1);
    }
}

void fillScaleComponentTask(void* data){
    UBYTE err;
    struct FillScaleTaskOpts* opts = (struct FillScaleTaskOpts*)data;
    struct Scale* scale = opts->scale;

    while(1){
        if(isScaleComponentFull(opts)){
            status("Scale %d Component %c is fully loaded!", scale->id, opts->componentName);
            opts->status = 0;
            wait(1);
            continue;
        }
        opts->status = 1;
        OSSemPend(scale->semaphore, 0, &err);
        if(err){
            printy(0, 1, "UNKNOWN ERROR! Terminating");
            exit(1);
        }
        opts->status = 1;
        fillScaleComponent(opts);
        OSSemPost(scale->semaphore);
        wait(1);
    }
}



void fillScaleComponent(const struct FillScaleTaskOpts* opts){
    struct Scale* scale = opts->scale;
    switch(opts->componentName){
        case 'A':
            fill(&(scale->components->A), &(opts->componentLimit));
            break;
        case 'B':
            fill(&(scale->components->B), &(opts->componentLimit));
            break;
        case 'C':
            fill(&(scale->components->C), &(opts->componentLimit));
            break;
        default:
            status("ERROR! UNKOWN COMPONENT NAME!");
            exit(1);
    }
}

void fill(int* a, const int* b){
    while(*a < *b){
        *a += 10;
        status("FILLING: %d/%d", *a, *b);
        wait(1);
    }
}

byte isScaleComponentFull(const struct FillScaleTaskOpts* opts){
    switch(opts->componentName){
         case 'A':
            if(opts->scale->components->A == opts->componentLimit){
                return 1;
            }
            return 0;
        case 'B':
            if(opts->scale->components->B == opts->componentLimit){
                return 1;
            }
            return 0;
        case 'C':
             if(opts->scale->components->C == opts->componentLimit){
                return 1;
            }
            return 0;
        default:
            status("ERROR! Check isScaleComponentFull failed!");
            return 0;
    }
}

void displayScaleInfo(const struct Scale* scale, const struct Components* components){
    printy(scale->infP->x, scale->infP->y, "[Scale %d]", scale->id);
    printy(scale->infP->x, scale->infP->y + 1 , "[Component A]: %d|%d", scale->components->A, components->A);
    printy(scale->infP->x, scale->infP->y + 2 , "[Component B: %d|%d", scale->components->B, components->B);
    printy(scale->infP->x, scale->infP->y + 3, "[Component C: %d|%d", scale->components->C, components->C);
}  

void displayFillingComponentInfo(const struct FillScaleTaskOpts* opts){
    printy(opts->infP.x, opts->infP.y, "[Filling Task]");
    printy(opts->infP.x, opts->infP.y + 1, "Target:Scale%d", opts->scale->id);
    printy(opts->infP.x, opts->infP.y + 2, "Component:%c", opts->componentName);
    printy(opts->infP.x, opts->infP.y + 3, "Status:%s", opts->status == 0 ? "waiting" : "loading");
}

void displayMixer(const struct Mixer* mixer, int total){
    printy(mixer->infP.x, mixer->infP.y, "[MIXER]");
    printy(mixer->infP.x, mixer->infP.y + 1, "Load: %d|%d", mixer->load, total);
    printy(mixer->infP.x, mixer->infP.y + 2, "Status: %s", mixer->status == 0 ? "waiting" : "working");
}

int calcTotalLoad(struct Recipe* recipe){
    int totalWeight1 = recipe->weight1->A + recipe->weight1->B + recipe->weight1->C;
    int totalWeight2 = recipe->weight2->A + recipe->weight2->B + recipe->weight2->C;
    return totalWeight1 + totalWeight2;
}

struct Recipe* readRecipe(char* path){
    struct Recipe* recipe = (struct Recipe*)malloc(sizeof (struct Recipe));
    recipe->weight1 = (struct Components*)malloc(sizeof(struct Components));
    recipe->weight2 = (struct Components*)malloc(sizeof(struct Components));

    FILE* file;
    file = fopen(path, "r");
    fscanf(file, "weight1: %d, %d, %d\n", &(recipe->weight1->A), &(recipe->weight1->B), &(recipe->weight1->C));
    status("Read values of weight1: %d, %d, %d", recipe->weight1->A, recipe->weight1->B, recipe->weight1->C);

    fscanf(file, "weight2: %d, %d, %d\n", &(recipe->weight2->A), &(recipe->weight2->B), &(recipe->weight2->C));
    status("Read values of weight2: %d, %d, %d", recipe->weight2->A, recipe->weight2->B, recipe->weight2->C);

    fscanf(file, "water: %d\ndrymix: %d\nwetmix: %d\n", &(recipe->waterDuration), &(recipe->drymixDuration), &(recipe->wetmixDuration));
    status("Read values of water: %d, drymix: %d, wetmix: %d", recipe->waterDuration, recipe->drymixDuration, recipe->wetmixDuration);
    return recipe;
}