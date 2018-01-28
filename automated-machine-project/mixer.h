#ifndef MIXER_H
#define MIXER_H

#include "../common.h"
#include "scales.h"

enum STAGE{
    DRY_MIXING,
    WATERING,
    WET_MIXING,
    UNLOADING
};

struct Mixer{
    int load;
    int loadLimit;
    OS_EVENT* semaphore;
    struct Point infP;
    enum STAGE stage;
};

struct FillMixerTaskOpts{
    struct Scale* scale;
    struct Mixer* mixer;
    struct Components* limits;
};

struct MixingTaskOpts{
    struct Mixer* mixer;
    struct Point infP;
    int mixingDuration;
    char* mixingType;
};

struct UnloadMixerTaskOpts{
    struct Mixer* mixer;
    struct Point infP;
    int unloadingDuration;
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
        OSSemPend(mopts->mixer->semaphore, 0, &err);
        if(err){
            printy(0, 1, "UNKNOWN ERROR IN MIXINGTASK! Terminating");
            wait(5);
            exit(1);
        }
        if(mopts->mixer->load != mopts->mixer->loadLimit || mopts->mixer->stage != UNLOADING){
            OSSemPost(mopts->mixer->semaphore);
            wait(1);
            continue;
        }
        printy(mopts->infP.x, mopts->infP.y + 1, "Status: working...");
        while(mopts->mixer->load != 0){
            mopts->mixer->load -= 10;
            wait(1);
        }
        mopts->mixer->stage = DRY_MIXING;
        printy(mopts->infP.x, mopts->infP.y + 1, "Status: waiting...");
        OSSemPost(mopts->mixer->semaphore);
    }
}

void mixingTask(void* data){
    UBYTE err;
    struct MixingTaskOpts* mopts = (struct MixingTaskOpts*)data;
    int mixingSince = 0;
    while(1){
        OSSemPend(mopts->mixer->semaphore, 0, &err);
        if(err){
            printy(0, 1, "UNKNOWN ERROR IN MIXINGTASK! Terminating");
            wait(5);
            exit(1);
        }
        if(!isMixerFull(mopts->mixer)){
            OSSemPost(mopts->mixer->semaphore);
            printy(mopts->infP.x, mopts->infP.y, "[Mixing %s Task]", mopts->mixingType);
            printy(mopts->infP.x, mopts->infP.y + 1, "Status: waiting...");
            wait(1);
            continue;
        }
        if(!strcmp(mopts->mixingType, "wet") && mopts->mixer->stage != WET_MIXING){
            OSSemPost(mopts->mixer->semaphore);
            wait(1);
            continue;
        }else if(!strcmp(mopts->mixingType, "dry") && mopts->mixer->stage != DRY_MIXING){
            OSSemPost(mopts->mixer->semaphore);
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
        mopts->mixer->stage++;
        OSSemPost(mopts->mixer->semaphore);
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
            wait(1);
            continue;
        }
        unloadComponent(&(opts->scale->components->A), &(opts->mixer->load));
        unloadComponent(&(opts->scale->components->B), &(opts->mixer->load));
        unloadComponent(&(opts->scale->components->C), &(opts->mixer->load));
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