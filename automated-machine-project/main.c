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

struct ScaleTaskOpts{
    char componentName;
    struct Scale* scale;
    int componentLimit;
};

struct WatchTaskOpts{
    struct Scale* scale1;
    struct Scale* scale2;
    struct Recipe* recipe;
};

// FUNCTIONS DECLARATIONS
///////////////////////////////////////////////////////////////////////////////////
struct Recipe* readRecipe(char* path);
void initialTask(void* data);
void fillScaleComponentTask(void* data);
void watchTask(void* data);
byte isScaleComponentEmpty(const struct ScaleTaskOpts* opts);
void fillScaleComponent(const struct ScaleTaskOpts* opts);
void fill(int* a, const int* b);
void displayScaleInfo(const struct Scale* scale, const struct Components* components);

// GLOBAL VARIABLES
///////////////////////////////////////////////////////////////////////////////////
OS_STK  initialTaskStack[TASK_STACK_SIZE], 
        watchTaskStack[TASK_STACK_SIZE], 
        fillScaleComponentA1TaskStack[TASK_STACK_SIZE],
        fillScaleComponentB1TaskStack[TASK_STACK_SIZE];


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
    
    struct Recipe* recipe = NULL;
    struct Scale scale1, scale2;
    scale1.id = 1;
    scale1.semaphore = OSSemCreate(1);
    scale1.components = (struct Components*)malloc(sizeof(struct Components));
    scale1.components->A = scale1.components->B = scale1.components->C = 0;
    scale1.infP = (struct Point*)malloc(sizeof(struct Point));
    scale1.infP->x = 0;
    scale1.infP->y = 10;
    
    scale2.id = 2;
    scale2.semaphore = OSSemCreate(1);
    scale2.components = (struct Components*)malloc(sizeof(struct Components));
    scale2.components->A = scale2.components->B = scale2.components->C = 0;
    scale2.infP = (struct Point*)malloc(sizeof(struct Point));
    scale2.infP->x = 50;
    scale2.infP->y = 10;

    struct ScaleTaskOpts a1, b1;
    a1.componentName = 'A';
    a1.scale = &scale1;
    b1.componentName = 'B';
    b1.scale = &scale1;

    struct WatchTaskOpts wopts;
    wopts.scale1 = &scale1;
    wopts.scale2 = &scale2;

    while(1){
        if(PC_GetKey(&key)){
            if(key == KEY_ESC){
                status("Key ESC pressed");
                exit(0);
            }
            if(key == KEY_SPACE){
                status("Key SPACE pressed");
                status("Read recipe...");
                recipe = readRecipe(RECIPE_PATH);
                wopts.recipe = recipe;
                status("Recipe has been read");
                status("Start fillScaleComponentTask for scale 1");
                a1.componentLimit = recipe->weight1->A;
                b1.componentLimit = recipe->weight1->B;
                createTask(fillScaleComponentTask, (void*)&a1, &fillScaleComponentA1TaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(fillScaleComponentTask, (void*)&b1, &fillScaleComponentB1TaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
                createTask(watchTask, (void*)&wopts, &watchTaskStack[TASK_STACK_SIZE - 1], getNextFreePrio());
            }
        }
        wait(1);
    }
}

void watchTask(void* data){
    UBYTE err;
    struct WatchTaskOpts* opts = (struct WatchTaskOpts*)data;
    while(1){
        displayScaleInfo(opts->scale1, opts->recipe->weight1);
        displayScaleInfo(opts->scale2, opts->recipe->weight2);
        wait(1);
    }

}

void displayScaleInfo(const struct Scale* scale, const struct Components* components){
    UBYTE err;
    OSSemPend(scale->semaphore, 0, &err);
    status("GOT SEMAPHORE");
    if(err){
        printy(0, 1, "UNKNOWN ERROR! Terminating");
        wait(5);
        exit(1);
    }
    printy(scale->infP->x, scale->infP->y, "[Scale %d]", scale->id);
    printy(scale->infP->x, scale->infP->y + 1 , "[Component A]: %d|%d", scale->components->A, components->A);
    printy(scale->infP->x, scale->infP->y + 2 , "[Component B: %d|%d", scale->components->B, components->B);
    printy(scale->infP->x, scale->infP->y + 3, "[Component C: %d|%d", scale->components->C, components->C);
    OSSemPost(scale->semaphore);
}  

void fillScaleComponentTask(void* data){
    UBYTE err;
    struct ScaleTaskOpts* opts = (struct ScaleTaskOpts*)data;
    struct Scale* scale = opts->scale;

    while(1){
        if(!isScaleComponentEmpty(opts)){
            status("Scale %d Component %c is not empty!", scale->id, opts->componentName);
            continue;
        }
        OSSemPend(scale->semaphore, 0, &err);
        if(err){
            printy(0, 1, "UNKNOWN ERROR! Terminating");
            exit(1);
        }
        fillScaleComponent(opts);
        status("Scale %d Component %c has been filled!", scale->id, opts->componentName);
        OSSemPost(scale->semaphore);
        wait(1);
    }
}

void fillScaleComponent(const struct ScaleTaskOpts* opts){
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

byte isScaleComponentEmpty(const struct ScaleTaskOpts* opts){
    switch(opts->componentName){
         case 'A':
            if(opts->scale->components->A == 0){
                return 1;
            }
            return 0;
        case 'B':
            if(opts->scale->components->B == 0){
                return 1;
            }
            return 0;
        case 'C':
             if(opts->scale->components->C == 0){
                return 1;
            }
            return 0;
        default:
            return 1;
    }
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