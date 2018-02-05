#ifndef WATER_H
#define WATER_H

#include "../common.h"
#include "mixer.h"

struct WaterTaskOpts{
    struct Mixer* mixer;
    struct Point infP;
    int wateringDuration;
    OS_EVENT* nativeSemaphore;
    OS_EVENT* externalSemaphore;
};

void wateringTask(void* data){
    UBYTE err;
    struct WaterTaskOpts* wopts = (struct WaterTaskOpts*)data;
    int wateringSince = 0;
    while(1){
        OSSemPend(wopts->nativeSemaphore, 1, &err);
        if(err){
            printy(wopts->infP.x, wopts->infP.y, "[Watering Task]");
            printy(wopts->infP.x, wopts->infP.y + 1, "Status: waiting...");
            wait(1);
            continue;
        }
        OSSemPend(wopts->mixer->semaphore, 0, &err);
        if(err){
            status("Error in watering task. Mixer semaphore is not available");
            wait(5);
            exit(1);
        }
        printy(wopts->infP.x, wopts->infP.y + 1, "Status: working...");
        while(wateringSince++ <= wopts->wateringDuration){
            wait(1);
        }
        wateringSince = 0;
        printy(wopts->infP.x, wopts->infP.y + 1, "Status: waiting...");
        OSSemPost(wopts->mixer->semaphore);
        OSSemPost(wopts->externalSemaphore);
    }
}

#endif