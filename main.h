#include <time.h> 

#ifndef PAGINGANDREPLACEMENT_MAIN_H
#define PAGINGANDREPLACEMENT_MAIN_H


void fifoPolicy(int *pageReferences, int *residentSet, int residentSetSize);

void lruPolicy(int *pageReferences, int *residentSet, int residentSetSize);

void clockPolicy(int *pageReferences, int *residentSet, int residentSetSize);

int getPageResidentSetIndex(int pageReference, int *residentSet, int residentSetSize);

int loadIfNotFull(int page , int *residentSet , int residentSetSize);

void printOutput(bool pageFault, int pageReference, int *residentSet, int residentSetSize);

int getLRUpageIndex(clock_t *recentlyUsed , int residentSetSize);

#endif //PAGINGANDREPLACEMENT_MAIN_H
