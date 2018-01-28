#ifndef WATER_H
#define WATER_H

#include "../common.h"
#include "mixer.h"

struct WaterTaskOpts{
    struct Mixer* mixer;
    struct Point infP;
    int wateringDuration;
};

void wateringTask(void* data){
    UBYTE err;
    struct WaterTaskOpts* wopts = (struct WaterTaskOpts*)data;
    int wateringSince = 0;
    while(1){
        if(!isMixerFull(wopts->mixer)){
            printy(wopts->infP.x, wopts->infP.y, "[Watering Task]");
            printy(wopts->infP.x, wopts->infP.y + 1, "Status: waiting...");
            wait(1);
            continue;
        }
        OSSemPend(wopts->mixer->semaphore, 0, &err);
        if(err){
            printy(0, 1, "UNKNOWN ERROR IN MIXINGTASK! Terminating");
            wait(5);
            exit(1);
        }
        if(wopts->mixer->stage != WATERING){
            OSSemPost(wopts->mixer->semaphore);
            wait(1);
            continue;
        }
        status("Increment watering stage");
        printy(wopts->infP.x, wopts->infP.y + 1, "Status: working...");
        while(wateringSince++ <= wopts->wateringDuration){
            wait(1);
        }
        wateringSince = 0;
        wopts->mixer->stage++;
        printy(wopts->infP.x, wopts->infP.y + 1, "Status: waiting...");
        OSSemPost(wopts->mixer->semaphore);
    }
}

#endif