#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "disc.h"

struct read_monitor{
    int block_number;
    void* buffer_addr;
    int request_time;
    int receipt_time;
    int completion_time;
} rm;

struct write_monitor{
    int block_number;
    void* buffer_addr;
    int request_time;
    int receipt_time;
    int completion_time;
} wm;

int my_clock = 0;

void *disc_start(void *args){

    printf("I am disc:%d\n",(*(int *) args));

    for(;;){
        //No-op
        //printf("Thread:%u, Num:%d\n",pthread_self(),(*(int *) args));
    }
}

int read(){
    /*
    int block_no;
    void* buf_addr;
    int request_time;
    int receipt_time;
    int completion_time;
    */

    //ask monitor for block number, buffer address and request time

    //PTHREAD_MUTEX_ERRORCHECKER

    if(rm.request_time > my_clock){
        my_clock = rm.request_time;
    }

    rm.receipt_time = my_clock;

    //This is pretend??
    //read(fd,buf_addr,4000);

    my_clock = 10 + 12 * drand48();

    rm.completion_time = my_clock;

    //update monitor with receipt time and completion time

    //do anything else it needs to let requestor know to proceed

    return 0;
}

int write(){
    /*
    int block_no;
    void* block_addr;
    int request_time;
    int receipt_time;
    int completion_time;
    */

    //ask monitor for block number, buffer address and request time

    if(wm.request_time > my_clock){
        my_clock = wm.request_time;
    }

    wm.receipt_time = my_clock;

    //This is pretend??
    //write(fd,block_addr,4000);

    my_clock = 10 + 12 * drand48();

    wm.completion_time = my_clock;

    //update monitor with receipt time and completion time

    //do anything else it needs to let requestor know to proceed

    return 0;
}

int quit(){
    //Exit thread
    return 0;
}
