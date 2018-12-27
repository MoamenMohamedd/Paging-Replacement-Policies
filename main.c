#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "main.h"


int main() {

    int residentSetSize;
    char pageReplacementPolicy[10];

    //read number of available frames for this process
    scanf("%d", &residentSetSize);

    //read which page replacement policy we should use
    scanf("%s", pageReplacementPolicy);

    //read all page references
    int size = 10;
    int *pageReferences = (int *) malloc(size * sizeof(int));
    int i = 0, pageRef;
    while (1) {
        scanf("%d", &pageRef);

        if (i == size) {
            size += 10;
            pageReferences = (int *) realloc(pageReferences, size * sizeof(int));
        }

        if (pageRef == -1) {
            pageReferences[i] = pageRef;
            break;
        }

        pageReferences[i] = pageRef;

        i++;
    }

    //initialize resident set for the process
    int *residentSet = (int *) malloc(residentSetSize * sizeof(int));
    for (int i = 0; i < residentSetSize; i++) {
        residentSet[i] = -1; //-1 for an empty frame
    }

    //run the policy
    if (strcmp(pageReplacementPolicy, "FIFO") == 0)
        fifoPolicy(pageReferences, residentSet, residentSetSize);
    else if (strcmp(pageReplacementPolicy, "LRU") == 0)
        lruPolicy(pageReferences, residentSet, residentSetSize);
    else if (strcmp(pageReplacementPolicy, "CLOCK") == 0)
        clockPolicy(pageReferences, residentSet, residentSetSize);


    free(residentSet);
    free(pageReferences);

    return 0;
}

void clockPolicy(int *pageReferences, int *residentSet, int residentSetSize) {

    puts("Replacement Policy = CLOCK");
    puts("-------------------------------------");
    puts("Page   Content of Frames");
    puts("----   -----------------");

    //count number of page faults excluding ones when resident set is not yet full
    int numPageFaults = 0;

    //clock policy is an approximation of lru policy
    //each frame is associated with a use bit that is set or unset when
    //the contained page is used or newly allocated
    //for simplicity i will use booleans instead of bits
    bool use[residentSetSize];

    //pointer to the frame that contains the page that should be replaced
    int pointer = 0;

    //serve page references
    int i = 0, numFreeFrames = residentSetSize;

    //page faults when process pages are first loaded don't count
    //also we need not to check if the frame's page use bit is set or not
    //since the frame would be empty anyways
    while (pageReferences[i] != -1 && numFreeFrames != 0) {

        //checks if the page exists in resident set if so return it's location
        int location = getPageResidentSetIndex(pageReferences[i], residentSet, residentSetSize);

        //page exist
        if (location != -1) {
            printOutput(false, pageReferences[i], residentSet, residentSetSize);

            //set use bit
            use[location] = true;
        } else {
            //page doesn't exist
            //allocate page to free frame but don't increase fault count
            int allocatedLocation = loadIfNotFull(pageReferences[i], residentSet, residentSetSize);

            //if page is allocated a free frame
            if (allocatedLocation != -1) {
                //decrease free frames
                numFreeFrames--;

                //set it's use bit
                use[allocatedLocation] = true;

                //update pointer
                pointer = (pointer + 1) % residentSetSize;

                printOutput(false, pageReferences[i], residentSet, residentSetSize);

            }
        }
        i++;

    }

    //resident set is full (begin replacing pages)
    while (pageReferences[i] != -1) { // not end of input

        //checks if the page exists in resident set if so return it's location
        int location = getPageResidentSetIndex(pageReferences[i], residentSet, residentSetSize);

        //page exist
        if (location != -1) {

            //set use bit
            use[location] = true;

            //page exists no fault occurred
            printOutput(false, pageReferences[i], residentSet, residentSetSize);

        } else {

            //signal a page fault
            //get page from virtual memory

            //choose page to replace
            pointer = getRepPageClock(pointer, use, residentSetSize);

            //replace with new page
            residentSet[pointer] = pageReferences[i];

            //update use bit for page
            use[pointer] = true;

            //update pointer
            pointer = (pointer + 1) % residentSetSize;

            //increase page fault count
            numPageFaults++;

            printOutput(true, pageReferences[i], residentSet, residentSetSize);

        }

        i++;
    }


    puts("-------------------------------------");
    printf("Number of page faults = %d\n", numPageFaults);

}

void lruPolicy(int *pageReferences, int *residentSet, int residentSetSize) {
    puts("Replacement Policy = LRU");
    puts("-------------------------------------");
    puts("Page   Content of Frames");
    puts("----   -----------------");

    //count number of page faults excluding ones when resident set is not yet full
    int numPageFaults = 0;

    //store time when pages are used
    //each frame is associated with a time that is updated every time 
    //the contained page is used or newly allocated
    clock_t recentlyUsed[residentSetSize];

    //serve page references
    int i = 0, numFreeFrames = residentSetSize;

    //page faults when process pages are first loaded don't count
    while (pageReferences[i] != -1 && numFreeFrames != 0) {

        //checks if the page exists in resident set if so return it's location
        int location = getPageResidentSetIndex(pageReferences[i], residentSet, residentSetSize);

        //page exist
        if (location != -1) {
            printOutput(false, pageReferences[i], residentSet, residentSetSize);

            //update time when it is used
            recentlyUsed[location] = clock();
        } else {
            //page doesn't exist
            //allocate page to free frame but don't increase fault count
            int allocatedLocation = loadIfNotFull(pageReferences[i], residentSet, residentSetSize);

            //if page is allocated a free frame
            if (allocatedLocation != -1) {
                //decrease free frames
                numFreeFrames--;

                //update time when page is used
                recentlyUsed[allocatedLocation] = clock();

                printOutput(false, pageReferences[i], residentSet, residentSetSize);

            }
        }
        i++;

    }

    //resident set is full (begin replacing pages)
    while (pageReferences[i] != -1) { // not end of input

        //checks if the page exists in resident set if so return it's location
        int location = getPageResidentSetIndex(pageReferences[i], residentSet, residentSetSize);

        //page exist
        if (location != -1) {

            //update time when page is used
            recentlyUsed[location] = clock();

            //page exists no fault occurred
            printOutput(false, pageReferences[i], residentSet, residentSetSize);

        } else {

            //signal a page fault
            //get page from virtual memory

            //choose lru page to replace
            int lruPageIndex = getLRUpageIndex(recentlyUsed, residentSetSize);

            //replace with new page
            residentSet[lruPageIndex] = pageReferences[i];

            //update time for the new page
            recentlyUsed[lruPageIndex] = clock();

            //increase page fault count
            numPageFaults++;

            printOutput(true, pageReferences[i], residentSet, residentSetSize);

        }

        i++;
    }


    puts("-------------------------------------");
    printf("Number of page faults = %d\n", numPageFaults);
}

void fifoPolicy(int *pageReferences, int *residentSet, int residentSetSize) {
    puts("Replacement Policy = FIFO");
    puts("-------------------------------------");
    puts("Page   Content of Frames");
    puts("----   -----------------");

    int fifoFrameToReplace = 0; //to keeptrack of the frame that will be chosen next for replacement
    int numPageFaults = 0; //count number of page faults excluding ones when resident set is not yet full


    //serve page references
    int i = 0, numFreeFrames = residentSetSize;

    //page faults when process pages are first loaded don't count
    while (pageReferences[i] != -1 && numFreeFrames != 0) {



        //page exists
        if (getPageResidentSetIndex(pageReferences[i], residentSet, residentSetSize) != -1) {
            printOutput(false, pageReferences[i], residentSet, residentSetSize);
        } else {
            //page doesn't exist
            //load it into a free frame and don't increase fault count
            if (loadIfNotFull(pageReferences[i], residentSet, residentSetSize) != -1) {
                //decrease free frames
                numFreeFrames--;

                //increase fifoFrameToReplace in a circular way
                fifoFrameToReplace = (fifoFrameToReplace + 1) % residentSetSize;

                printOutput(false, pageReferences[i], residentSet, residentSetSize);

            }
        }
        i++;

    }

    //resident set is full (begin replacing pages)
    while (pageReferences[i] != -1) { // not end of input

        if (getPageResidentSetIndex(pageReferences[i], residentSet, residentSetSize) != -1) {

            //page exists no fault occurred
            printOutput(false, pageReferences[i], residentSet, residentSetSize);

        } else {

            //signal a page fault
            //get page from virtual memory
            //allocate page to frame
            residentSet[fifoFrameToReplace] = pageReferences[i];

            //increase fifoFrameToReplace in a circular way
            fifoFrameToReplace = (fifoFrameToReplace + 1) % residentSetSize;

            //increase page fault count
            numPageFaults++;

            printOutput(true, pageReferences[i], residentSet, residentSetSize);

        }

        i++;
    }

    puts("-------------------------------------");
    printf("Number of page faults = %d\n", numPageFaults);
}

/**
 * checks weather the referenced page exists in resident set or not
 * and returns it's location
 * */
int getPageResidentSetIndex(int pageReference, int *residentSet, int residentSetSize) {

    int i = 0;
    while (i < residentSetSize && residentSet[i] != -1) {
        if (residentSet[i] == pageReference)
            return i;
        i++;
    }
    return -1;
}

/**
 * prints the current state of resident set after a page reference has occurred
 * */
void printOutput(bool pageFault, int pageReference, int *residentSet, int residentSetSize) {

    if (pageFault)
        printf("%02d F   ", pageReference);
    else
        printf("%02d     ", pageReference);

    for (int i = 0; i < residentSetSize; i++) {
        if (residentSet[i] != -1)
            printf("%02d ", residentSet[i]);
    }

    printf("\n");
}

/**
 * loads process page if the resident set is not yet full
 * and returns it's location
 * */
int loadIfNotFull(int page, int *residentSet, int residentSetSize) {

    for (int i = 0; i < residentSetSize; i++) {
        if (residentSet[i] == -1) { //empty frame
            residentSet[i] = page; //assign frame to page
            return i;
        }

    }
    return -1;
}


int getLRUpageIndex(clock_t *recentlyUsed, int residentSetSize) {
    clock_t min = recentlyUsed[0];
    int lruLocation = 0;

    for (int i = 1; i < residentSetSize; i++) {
        if (recentlyUsed[i] < min) {
            min = recentlyUsed[i];
            lruLocation = i;
        }
    }

    return lruLocation;
}

int getRepPageClock(int pointerPlace, bool *useArray, int residentSetSize) {

    //start from last index of the pointer
    int i = pointerPlace;
    while (useArray[i] == true) {
        useArray[i] = false;
        i = (i + 1) % residentSetSize;
    }

    return i;
}