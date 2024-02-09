#include <stdlib.h>
#include <stdio.h>
#include "mymalloc.h"
#define MEMLENGTH 512
#define UNALLOCATED 0
#define ALLOCATED 1
#define STATUS 0
#define PAYSIZE 1
#define HEADERSIZE 8

static double memory[MEMLENGTH];
char *heapstart = (char *) memory;

void *mymalloc(size_t size, char *file, int line){
    int initialized = 0;
    for(int i = 0; i < MEMLENGTH; i++){
        if (memory[i] != 0){
            initialized ++;
        }
        
    }
    if(initialized == 0){
        int *metastart = (int *) memory;
        *(metastart + STATUS) = UNALLOCATED;
        *(metastart + PAYSIZE) = 4088;
    }
    char *paypointer = (char *) memory;
    paypointer += 8;
    printf(" *(heapstart + 4): %d\n", * (int *) (heapstart + 4));
    printf(" * (int *) (paypointer-4): %d\n", * (int *) (paypointer-4));
    
    //printf("%d\n",  * (int *) (paypointer-4));
    //printf("%d\n",  * (int *) (paypointer-8));
    int i = 0;
    while(i < 4088){
        if(* (int *) (paypointer-4) >= (size + 16) && * (int *) (paypointer-8) == 0){
            int origsize = * (int *) (paypointer-4);
            * (int *) (paypointer-8) = ALLOCATED;
            * (int *) (paypointer-4) = size;
            * (int *) (paypointer+size) = UNALLOCATED;
            * (int *) (paypointer+size+4) = origsize - size - HEADERSIZE;
            i+=4088;
        }
        //printf("%d\n",  * (int *) (paypointer-4));
        i += * (int *) (paypointer-4);
        paypointer += * (int *) (paypointer-4);
        
        //i += * (int *) (paypointer-4);
    }
    return (void*) memory;

}

int main(){
    //printf("%p", malloc(8));
    //printf("%p", malloc(8));
    //malloc(8);
    //int *metastart = (int *) memory;
    //printf("%d\n", *metastart);
    //printf("%d\n", *(metastart+1));
    //printf("%ld\n", sizeof(int));

    /*
    Test malloc function: call malloc(8): this should set the first header to be allocated with a size of 8. 
    expected: 
    * (int *) (paypointer - 8) = 1
    * (int *) (paypointer - 4) = 8
    A new header should be made at heapstart+8 to be unallocated with a size of 4088 - 16
    expected: 
    * (int *) (paypointer + 8) = 0
    * (int *) (paypointer + 12) = 4072
    */
    malloc(8);
    char *paypointer = (char *) memory;
    paypointer += 8;
    printf(" * (int *) (paypointer - 8): %d\n", * (int *) (paypointer - 8));
    printf(" * (int *) (paypointer - 4): %d\n", * (int *) (paypointer - 4));
    printf(" * (int *) (paypointer + 8): %d\n", * (int *) (paypointer + 8));
    printf(" * (int *) (paypointer + 12): %d\n", * (int *) (paypointer + 12));
}