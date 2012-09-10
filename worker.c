#include <stdlib.h>
#include <stdio.h>
#include "worker.h"

void *work(void *L){
    int i = 0;
    int o = 0;
    int F = 1000;
    int count = 0;
    int rep = 0;
    int clock = 0;

    printf("IamA Worker %d\n",*((int *) L));

    while(i == o){
        i = 1000 * drand48();
        o = 1000 * drand48();
    }

    if(i < o){
        //obtain a read lock on file i
        //obtain a write lock on file o
    } else {
        //obtain a write lock on file o
        //obtain a read lock on file i
    }

    //read from block 0 of file i
    //write at block 0 of file o

    for(count = 0; count <= 125000; count++){
        //read a 1 kiB record from input file i
        //write that 1 kiB record to output file o
    }

    //release write lock on file o
    //release read lock on file i

    return;
}
