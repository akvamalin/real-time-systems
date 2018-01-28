#ifndef MIXER_H
#define MIXER_H

#include "../common.h"
#include "scales.h"

struct Mixer{
    int load;
    int loadLimit;
    OS_EVENT* semaphore;
    struct Point infP;
};

struct FillMixerTaskOpts{
    struct Scale* scale;
    struct Mixer* mixer;
    struct Components* limits;
    OS_EVENT* nativeSemaphore;
    OS_EVENT* externalSemaphore;
};

struct MixingTaskOpts{
    struct Mixer* mixer;
    struct Point infP;
    int mixingDuration;
    char* mixingType;
    OS_EVENT* nativeSemaphore;
    OS_EVENT* externalSemaphore;
};

struct UnloadMixerTaskOpts{
    struct Mixer* mixer;
    struct Point infP;
    int unloadingDuration;
    OS_EVENT* nativeSemaphore;
    OS_EVENT* externalSemaphore;
};

void mixingTask(void* data);
void fillMixerTask(void* data);
byte isMixerFull(const struct Mixer* mixer);
void displayMixer(const struct Mixer* mixer, int total);
void unloadComponent(int* source, int* destination);
void unloadMixerTask(void* data);

void unloadMixerTask(void* data){
    UBYTE err;
    struct UnloadMixerTaskOpts* mopts = (struct UnloadMixerTaskOpts*)data;
    printy(mopts->infP.x, mopts->infP.y, "[Unloading Mixer Task]");
    printy(mopts->infP.x, mopts->infP.y + 1, "Status: waiting...");
    while(1){
        OSSemPend(mopts->nativeSemaphore, 1, &err);
        if(err){
            printy(mopts->infP.x, mopts->infP.y + 1, "Status: waiting...");
            wait(1);
            continue;
        }
        OSSemPend(mopts->mixer->semaphore, 0, &err);
        if(err){
            printy(0, 1, "A UNKNOWN ERROR in MIXER! Terminating");
            wait(5);
            exit(1);
        }
        printy(mopts->infP.x, mopts->infP.y + 1, "Status: working...");
        while(mopts->mixer->load != 0){
            mopts->mixer->load -= 10;
            wait(1);
        }
        printy(mopts->infP.x, mopts->infP.y + 1, "Status: waiting...");
        OSSemPost(mopts->mixer->semaphore);
        OSSemPost(mopts->externalSemaphore);
    }
}

void mixingTask(void* data){
    UBYTE err;
    struct MixingTaskOpts* mopts = (struct MixingTaskOpts*)data;
    int mixingSince = 0;
    while(1){
        OSSemPend(mopts->nativeSemaphore, 1, &err);
        if(err){
            printy(mopts->infP.x, mopts->infP.y, "[Mixing %s Task]", mopts->mixingType);
            printy(mopts->infP.x, mopts->infP.y + 1, "Status: waiting...");
            wait(1);
            continue;
        }
        if(!isMixerFull(mopts->mixer)){
            printy(mopts->infP.x, mopts->infP.y, "[Mixing %s Task]", mopts->mixingType);
            printy(mopts->infP.x, mopts->infP.y + 1, "Status: waiting...");
            wait(1);
            continue;
        }

        printy(mopts->infP.x, mopts->infP.y + 1, "Status: working...");
        printy(mopts->infP.x, mopts->infP.y + 2, EMPTY_STRING);
        while(mixingSince++ <= mopts->mixingDuration){
            wait(1);
        }
        mixingSince = 0;
        printy(mopts->infP.x, mopts->infP.y + 1, "Status: waiting...");
        OSSemPost(mopts->externalSemaphore);
    }
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
        if(!isScaleFull(opts->scale->components, opts->limits)){
            OSSemPost(opts->scale->semaphore);
            wait(1);
            continue;
        }
        OSSemPend(opts->mixer->semaphore, 0, &err);
        if(err){
            printy(0, 1, "B UNKNOWN ERROR in MIXER! Terminating");
            wait(1);
            exit(1);
        }
        if(opts->mixer->load >= opts->mixer->loadLimit){
            OSSemPost(opts->mixer->semaphore);
            OSSemPost(opts->scale->semaphore);
            wait(1);
            continue;
        }
        unloadComponent(&(opts->scale->components->A), &(opts->mixer->load));
        unloadComponent(&(opts->scale->components->B), &(opts->mixer->load));
        unloadComponent(&(opts->scale->components->C), &(opts->mixer->load));
        if(opts->mixer->load >= opts->mixer->loadLimit){
            OSSemPost(opts->externalSemaphore);   
        }
        OSSemPost(opts->mixer->semaphore);
        OSSemPost(opts->scale->semaphore);
        wait(1);
    }
}

byte isMixerFull(const struct Mixer* mixer){
    return mixer->load == mixer->loadLimit ? 1 : 0;
}

void displayMixer(const struct Mixer* mixer, int total){
    printy(mixer->infP.x, mixer->infP.y, "[MIXER]");
    printy(mixer->infP.x, mixer->infP.y + 1, "Load: %3d|%3d", mixer->load, total);
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