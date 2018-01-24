/*
    Yevhenii Maliavka
    BAIN5
    Hochschule Merseburg
*/

#include "../common.h"

#define MAX_BUFFER_SIZE 256
#define TASK_STACK_SIZE 512

// INITIAL TASK COORDINATES
#define POS_X_INITIAL_TASK 35
#define POS_Y_INITIAL_TASK 6

#define RECIPE_PATH "recipe.txt"

struct Components{
    int A;
    int B;
    int C;
};

struct Recipe{
    struct Components* weight1;
    struct Components* weight2;
    int waterDuration;
    int drymixDuration;
    int wetmixDuration;
};

OS_STK initialTaskStack[TASK_STACK_SIZE];

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

void initialTask(void* data){    
    INT16S key;
    printy(POS_X_INITIAL_TASK, POS_Y_INITIAL_TASK, "[INITIAL TASK]: working...");
    struct Recipe* recipe = NULL;

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
                status("Recipe has been read");
                wait(1);
            }
        }
        wait(1);
    }
}

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