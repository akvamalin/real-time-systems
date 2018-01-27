#ifndef MIXER_H
#define MIXER_H

#include "../common.h"
#include "scales.h"

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

void fillMixerTask(void* data);
void displayMixer(const struct Mixer* mixer, int total);
void unloadComponent(int* source, int* destination);

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

void displayMixer(const struct Mixer* mixer, int total){
    printy(mixer->infP.x, mixer->infP.y, "[MIXER]");
    printy(mixer->infP.x, mixer->infP.y + 1, "Load: %3d|%3d", mixer->load, total);
    printy(mixer->infP.x, mixer->infP.y + 2, "Status: %s", mixer->status == 0 ? "waiting" : "working");
}

void unloadComponent(int* source, int* destination){
    int loadCapacity = 10;
    while(*source){
        *destination += loadCapacity;
        *source -= loadCapacity;
        wait(1);
    }
}

#endif