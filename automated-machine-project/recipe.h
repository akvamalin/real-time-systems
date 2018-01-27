#ifndef RECIPE_H
#define RECIPE_H

#include <stdio.h>
#include "scales.h"

struct Recipe{
    struct Components* weight1;
    struct Components* weight2;
    int waterDuration;
    int drymixDuration;
    int wetmixDuration;
};

struct Recipe* readRecipe(char* path);

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

#endif