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
    size = (size + 7) & ~7;
    char *paypointer = (char *) memory;
    paypointer += 8;
    int i = 0;
    while(i < 4088){
        if(* (int *) (paypointer-4) >= (size /*+ 16*/) && * (int *) (paypointer-8) == 0){
            int origsize = * (int *) (paypointer-4);
            * (int *) (paypointer-8) = ALLOCATED;
            * (int *) (paypointer-4) = size;
            if (origsize - size >= 16 && origsize - size - HEADERSIZE != 0){
                * (int *) (paypointer+size) = UNALLOCATED;
                * (int *) (paypointer+size+4) = origsize - size - HEADERSIZE;
            }
            /*printf(" * (int *) (paypointer - 8): %d\n", * (int *) (paypointer - 8));
            printf(" * (int *) (paypointer - 4): %d\n", * (int *) (paypointer - 4));
            printf(" * (int *) (paypointer + size): %d\n", * (int *) (paypointer + size));
            printf(" * (int *) (paypointer + size+4): %d\n\n", * (int *) (paypointer + size + 4));*/
            return paypointer;
            i+=4088;
        }

        paypointer += (* (int *) (paypointer - 4) + 8);
        i += * (int *) (paypointer-4);
    }
    void *vptr;
    printf("Not enough space, void pointer returned.");
    return vptr;

}

int checkfree(void *mall){
    char *sizeptr = (char *) memory;
    sizeptr += 4;
    int i = 4;
    /*printf("%p\n", sizeptr);
    printf("%p\n", mall);
    printf("%d\n", mall == (sizeptr));*/
    do{
        if((sizeptr + 4) == mall && * (int *) (sizeptr - 4) == 1) {
            return 1;
            i += 4088;
        }
        if((sizeptr + 4) == mall && * (int *) (sizeptr - 4) == 0){
            //printf("Tried to free a freed pointer");
            return 2;
        }
        i += * (int *) (sizeptr) + 8;
        sizeptr += * (int *) (sizeptr) + 8;
    } while(i < 4084);

    //printf("The pointer has not been malloc'd");
    return 0;

} // Checks whether the address that is to be freed has been malloc'd or not. 1=true 0=false

void printArray(){
    int i = 0;
    char *ptr = (char *) memory;
    do{
        //printf("Block %d\n", i);
        if(checkfree(ptr+HEADERSIZE) == 1 || checkfree(ptr+HEADERSIZE) == 2){
            printf("Allocated: %d\n", * (int*) (ptr));
            printf("Size: %d\n\n", *(int*) (ptr+4));
            i += *(int*) (ptr+4);
            ptr += 8 + (*(int*) (ptr+4));
        }
        else{
            i+= 4072;
            return;
        }
        
    } while (i<4072);
}

void coalesce(){
    char *prev = (char *) memory;
    char *ptr = prev + HEADERSIZE + (*(int*) (prev + 4));
    int i = * (int*) (prev + 4);
    do{
        while(* (int*) (prev) == 0 && * (int*) (ptr) == 0 && i<4088 ){
            * (int*) (prev + 4) += HEADERSIZE + * (int*) (ptr + 4);
            ptr = prev + HEADERSIZE + * (int*) (prev + 4);
            i += HEADERSIZE + * (int*) (prev + 4);  
            //printf("%d\n", i);
        }
        i += HEADERSIZE + * (int*) (ptr + 4);
        prev = ptr;
        ptr += HEADERSIZE + * (int*) (ptr + 4);
    } while (i < 4072);
}

void myfree(void *ptr, char *file, int line){
    //coalesce();
    if (checkfree(ptr) == 1){
        * (int *) (ptr - 8) = 0;
    }
    coalesce();
}

int main(){

    void *a = malloc(8);
    void *b = malloc(32);
    void *c = malloc(16);
    void *d = malloc(31);
    void *e = malloc(8);
    void *f = malloc(16);
    void *g = malloc (8);
    printArray();
    free(a);
    free(b);
    free(c);
    free(e);
    free(f);
    printf("========== free a, b, c, e, f\n");
    printArray();   
    coalesce();
    printf("========== coalesce \n"); 
    printArray();

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
    /*void *a = malloc(8);
    void *b = malloc(32);
    void *c = malloc(16);
    free(b);
    
    printArray();
    printf("============\n");
    //void *b2 = malloc(32);
    void *b2 = malloc(8);
    printArray();
    printf("============\n");
    void *b3 = malloc(8);
    printArray(); //should fit in before 16 byte payload
    printf("============\n");
    void *b4 = malloc(8);
    printArray(); //should end up after 16 byte payload*/



    /*printf("Heapstart: (allocated/unallocated): %d\n", * (int *) heapstart);
    printf("Heapstart + 4: (payload size): %d\n", * (int *) (heapstart+4));
    free(a);
    printf("Heapstart: (allocated/unallocated): %d\n", * (int *) heapstart);
    printf("Heapstart + 4: (payload size): %d\n", * (int *) (heapstart+4));*/
    /*int x = 5;
    printf("%d\n", checkfree(&x));*/
    
}