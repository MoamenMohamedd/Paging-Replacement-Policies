#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


void fifoPolicy(int *pageReferences , int numFrames, int *allocationArray);

void lruPolicy(int *pageReferences , int numFrames, int *allocationArray);

void clockPolicy(int *pageReferences , int numFrames, int *allocationArray);

bool pageExists(int pageReference, int *allocationArray, int numAllocatedFrames);

void printOutput(bool pageFault, int pageReference, int *allocationArray, int numAllocatedFrames) ;

int main() {

    int numFramesAllocatedToProcess;
    char pageReplacementPolicy[10];

    scanf("%d", &numFramesAllocatedToProcess);
    scanf("%s", pageReplacementPolicy);

    int size = 10;
    int *pageReferences = (int *) malloc( size * sizeof(int));
    int i = 0 , pageRef;
    while(1){
        scanf("%d" ,&pageRef);

        if(i == size){
            size += 10;
            pageReferences = (int*)realloc(pageReferences , size * sizeof(int));
        }

        if(pageRef == -1){
            pageReferences[i] = pageRef;
            break;
        }
            
        pageReferences[i] = pageRef;

        i++;
    }
        

    int *allocatedFrames = (int *) malloc(numFramesAllocatedToProcess * sizeof(int));
    for (int i = 0; i < numFramesAllocatedToProcess; i++) {
        allocatedFrames[i] = -1; //-1 for an empty frame
    }

    if (strcmp(pageReplacementPolicy, "FIFO") == 0)
        fifoPolicy(pageReferences , numFramesAllocatedToProcess, allocatedFrames);
    else if (strcmp(pageReplacementPolicy, "LRU") == 0)
        lruPolicy(pageReferences , numFramesAllocatedToProcess, allocatedFrames);
    else if (strcmp(pageReplacementPolicy, "CLOCK") == 0)
        clockPolicy(pageReferences , numFramesAllocatedToProcess, allocatedFrames);


    free(allocatedFrames);
    free(pageReferences);

    return 0;
}

void clockPolicy(int *pageReferences , int frames, int *allocationArray) {

}

void lruPolicy(int *pageReferences , int frames, int *allocationArray) {

}

void fifoPolicy(int *pageReferences , int numAllocatedFrames, int *allocationArray) {
    puts("Policy = FIFO");
    puts("-------------------------------------");
    puts("Page\tContent of Frames");
    puts("----\t-----------------");

    int nextFrameIndex = 0 , numPageFaults = 0;

    int i = 0;
    while (pageReferences[i] != -1) { // not end of input

    

            if (pageExists(pageReferences[i], allocationArray, numAllocatedFrames)) {

                printOutput(false , pageReferences[i] , allocationArray , numAllocatedFrames);

            } else {

                if(allocationArray[nextFrameIndex] == -1){
                    //count page faults only after all the frames are allocated
                    //get page from virtual memory
                    //allocate page to frame
                    allocationArray[nextFrameIndex] = pageReferences[i];

                    //increase nextFrameIndex in a circular way
                    nextFrameIndex = (nextFrameIndex + 1) % numAllocatedFrames;

                    printOutput(false , pageReferences[i] , allocationArray ,numAllocatedFrames);
                }else{
                    //signal a page fault
                    //get page from virtual memory
                    //allocate page to frame
                    allocationArray[nextFrameIndex] = pageReferences[i];

                    //increase nextFrameIndex in a circular way
                    nextFrameIndex = (nextFrameIndex + 1) % numAllocatedFrames;

                    numPageFaults++;

                    printOutput(true , pageReferences[i] , allocationArray ,numAllocatedFrames);
                }

            }
        

        i++;

    }

    puts("-------------------------------------");
    printf("Number of page faults = %d\n",numPageFaults);

}

bool pageExists(int pageReference, int *allocationArray, int numAllocatedFrames) {

    for (int i = 0; i < numAllocatedFrames; i++) {
        if (allocationArray[i] == pageReference)
            return true;
    }
    return false;
}

void printOutput(bool pageFault, int pageReference, int *allocationArray, int numAllocatedFrames) {

    if (pageFault)
        printf("%02d F\t", pageReference);
    else
        printf("%02d\t", pageReference);

    for (int i = 0; i < numAllocatedFrames; i++) {
        if (allocationArray[i] == -1)
            printf("   ");
        else
            printf("%02d ", allocationArray[i]);
    }

    printf("\n");
}