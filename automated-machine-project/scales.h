#ifndef SCALES_H
#define SCALES_H

#include "../common.h"

struct Components{
    int A;
    int B;
    int C;
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

void fillScaleComponentTask(void* data);
void fillScaleComponent(const struct FillScaleTaskOpts* opts);
void fill(int* a, const int* b);
void displayScaleInfo(const struct Scale* scale, const struct Components* components);
void displayFillingComponentInfo(const struct FillScaleTaskOpts* opts);
byte isScaleComponentFull(const struct FillScaleTaskOpts* opts);
byte isScaleFull(const struct Components* values, const struct Components* limits);

void fillScaleComponentTask(void* data){
    UBYTE err;
    struct FillScaleTaskOpts* opts = (struct FillScaleTaskOpts*)data;
    struct Scale* scale = opts->scale;

    while(1){
        if(isScaleComponentFull(opts)){
            wait(1);
            continue;
        }
        OSSemPend(scale->semaphore, 0, &err);
        if(err){
            printy(0, 1, "UNKNOWN ERROR! Terminating");
            wait(5);
            exit(1);
        }
        opts->status = 1;
        fillScaleComponent(opts);
        opts->status = 0;
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
        wait(1);
    }
}


void displayScaleInfo(const struct Scale* scale, const struct Components* components){
    printy(scale->infP->x, scale->infP->y, "[Scale %d]", scale->id);
    printy(scale->infP->x, scale->infP->y + 1 , "[Component A]: %3d|%3d", scale->components->A, components->A);
    printy(scale->infP->x, scale->infP->y + 2 , "[Component B: %3d|%3d", scale->components->B, components->B);
    printy(scale->infP->x, scale->infP->y + 3, "[Component C: %3d|%3d", scale->components->C, components->C);
}  

void displayFillingComponentInfo(const struct FillScaleTaskOpts* opts){
    printy(opts->infP.x, opts->infP.y, "[Filling Task]");
    printy(opts->infP.x, opts->infP.y + 1, "Target:Scale%d", opts->scale->id);
    printy(opts->infP.x, opts->infP.y + 2, "Component:%c", opts->componentName);
    printy(opts->infP.x, opts->infP.y + 3, "Status:%s", opts->status == 0 ? "waiting" : "loading");
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

byte isScaleFull(const struct Components* values, const struct Components* limits){
    if(values->A != limits->A || values->B != limits->B || values->C != limits->C){
        return 0;
    }
    return 1;
}

#endif