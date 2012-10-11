#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "tools.h"
#include "worker.h"
#include "assert.h"

#define BUF_SIZE 4096

void *work(void *args){
    int i = 0;
    int o = 0;
    int d = 0;
    int F = 1000;
    int count = 0;
    int blocks_per_file = 250000;
    //int work_id = *((int *) L);
    int work_id = 0;
    info thread_info = *((info *)(args));
    char read_buf[BUF_SIZE];
    char write_buf[BUF_SIZE];
    //Worker w starts w seconds after origin
    thread_info.work_times[work_id] = work_id;

    work_id = get_id(&thread_info, pthread_self());

    printf("IamA Worker %d\n",work_id);
        
    //For testing
    /*
    pthread_mutex_lock(&thread_info.read_mons[d].lock);
    write_circ_buf(&thread_info.read_queues[d], work_id);
    pthread_mutex_unlock(&thread_info.read_mons[d].lock);
    */

    while(i == o){
        i = F * drand48();
        o = F * drand48();
    }

    //printf("Input file:%d\n",i);
    //printf("Output file:%d\n",o);

/* TODO implement file locks?
    if(i < o){
        //obtain a read lock on file i
        pthread_mutex_lock(&thread_info.read_mons[work_id].lock);
        //obtain a write lock on file o
        pthread_mutex_lock(&thread_info.write_mons[work_id].lock);
    } else {
        //obtain a write lock on file o
        pthread_mutex_lock(&thread_info.write_mons[work_id].lock);
        //obtain a read lock on file i
        pthread_mutex_lock(&thread_info.read_mons[work_id].lock);
    }
*/
    //read from block 0 of file i
    //write at block 0 of file o

    for(count = 0; count < blocks_per_file; count++){
        d = 0;
        read_file(&thread_info,i,d,count,work_id, read_buf);
        write_file(&thread_info,o,d,count,work_id, write_buf);
    }

/*
    //release write lock on file o
    pthread_mutex_unlock(&thread_info.write_mons[work_id].lock);
    //release read lock on file i
    pthread_mutex_unlock(&thread_info.read_mons[work_id].lock);
*/
    return 0;
}

void read_file(void* thread_info, int i, int d, int count, int work_id, char* read_buf){
    /* Read a 1 kiB record from input file i */
    mon temp;
    info ti = *((info *)(thread_info));

    /* Lock read queue */
    pthread_mutex_lock(&ti.read_mons[d]);

    /* Create monitor */
    temp.block_number = count;
    temp.buffer_addr = read_buf;
    temp.request_time = ti.work_times[work_id];

    /* Write monitor to queue */
    write_circ_buf(&ti.read_queues[d], &temp);

    /* Unlock queue */
    pthread_mutex_unlock(&ti.read_mons[d]);

    /* Get back completion time from disc */
    /*
    for(;;){
        while(!(is_circ_empty(&thread_info.read_response[work_id]))){
            temp = read_circ_buf(&thread_info.read_queues[disc_id]);
            ti.work_times[work_id] = 
        }
    }*/
    

    //printf("Comp time:%d\n",ti.read_mons[d].completion_time);
    //ti.work_times[work_id] = ti.read_mons[d].completion_time;
}

void write_file(void* thread_info,int i, int d, int count, int work_id, char* write_buf){
    //write that 1 kiB record to output file o
    info ti = *((info *)(thread_info));
    /* Lock write queue */
    pthread_mutex_lock(&ti.write_mons[d]);

    mon temp;

    temp.block_number = count;
    temp.buffer_addr = write_buf;
    temp.request_time = ti.work_times[work_id];

    //increment time by 1 before write
    ti.work_times[work_id] = ti.work_times[work_id] + 1;

    write_circ_buf(&ti.write_queues[d], &temp);
    //printf("Reciept time:%d\n",ti.write_mons[d].completion_time);

    /* Unlock queue */
    pthread_mutex_unlock(&ti.write_mons[d]);

    //call writer
}
