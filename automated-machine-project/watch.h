#ifndef WATCH_H
#define WATCH_H

#include "../common.h"
#include "scales.h"
#include "recipe.h"

struct WatchTaskOpts{
    struct Scale* scale1;
    struct Scale* scale2;
    struct Recipe* recipe;
    struct FillScaleTaskOpts** fillingOpts;
    struct Mixer* mixer;
    int componentsCount;
};

void watchTask(void* data);

void watchTask(void* data){
    UBYTE err;
    struct WatchTaskOpts* opts = (struct WatchTaskOpts*)data;
    struct FillScaleTaskOpts** headFillingOpts = opts->fillingOpts;
    int i;
    int totalLoad = calcTotalLoad(opts->recipe);
    while(1){
        displayScaleInfo(opts->scale1, opts->recipe->weight1);
        displayScaleInfo(opts->scale2, opts->recipe->weight2);
        for(i = 0; i < opts->componentsCount; i++){
            displayFillingComponentInfo(*(opts->fillingOpts)++);
        }
        opts->fillingOpts = headFillingOpts;
        displayMixer(opts->mixer, totalLoad);
        wait(1);
    }
}

#endif