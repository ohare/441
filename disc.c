#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "disc.h"
#include "tools.h"

int my_clock = 0;

void *disc_start(void *args){

    printf("I am disc:%d\n",(*(int *) args));

    for(;;){
        //No-op
        //printf("Thread:%u, Num:%d\n",pthread_self(),(*(int *) args));
    }
}

int read(rm rmon){
    /*
    int block_no;
    void* buf_addr;
    int request_time;
    int receipt_time;
    int completion_time;
    */

    //ask monitor for block number, buffer address and request time

    //PTHREAD_MUTEX_ERRORCHECKER

    if(rmon.request_time > my_clock){
        my_clock = rmon.request_time;
    }

    rmon.receipt_time = my_clock;

    //This is pretend??
    //read(fd,buf_addr,4000);

    my_clock = 10 + 12 * drand48();

    rmon.completion_time = my_clock;

    //update monitor with receipt time and completion time

    //do anything else it needs to let requestor know to proceed

    return 0;
}

int write(wm wmon){
    /*
    int block_no;
    void* block_addr;
    int request_time;
    int receipt_time;
    int completion_time;
    */

    //ask monitor for block number, buffer address and request time

    if(wmon.request_time > my_clock){
        my_clock = wmon.request_time;
    }

    wmon.receipt_time = my_clock;

    //This is pretend??
    //write(fd,block_addr,4000);

    my_clock = 10 + 12 * drand48();

    wmon.completion_time = my_clock;

    //update monitor with receipt time and completion time

    //do anything else it needs to let requestor know to proceed

    return 0;
}

int quit(){
    //Exit thread
    return 0;
}
