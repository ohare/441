#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "worker.h"
#include "tools.h"

#define BUF_SIZE 4096

void *work(void *args){
    int i = 0;
    int o = 0;
    int d = 0;
    int F = 1000;
    int count = 0;
    int clock = 0;
    int blocks_per_file = 250000;
    //int work_id = *((int *) L);
    int work_id = 0;
    info thread_info = *((info *)(args));
    char read_buf[BUF_SIZE];
    char write_buf[BUF_SIZE];
    void *null = 0;

    printf("IamA Worker %d\n",work_id);
        
    pthread_mutex_lock(&thread_info.read_mons[d].lock);
    write_circ_buf(thread_info.read_queues[d], work_id);
    pthread_mutex_unlock(&thread_info.read_mons[d].lock);
    return null;

    while(i == o){
        i = F * drand48();
        o = F * drand48();
    }

    printf("Input file:%d\n",i);
    printf("Output file:%d\n",o);

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

    //read from block 0 of file i
    //write at block 0 of file o

    for(count = 0; count <= blocks_per_file; count++){
        //read a 1 kiB record from input file i
        d = 0;
        thread_info.read_mons[work_id].block_number = count;
        thread_info.read_mons[work_id].buffer_addr = &read_buf;
        thread_info.read_mons[work_id].request_time = clock;
        //printf("Worker:%d, writing to read_queues[%d], value:%d\n",work_id,d,work_id);
        write_circ_buf(thread_info.read_queues[d], work_id);
        //get back time from disk

        //write that 1 kiB record to output file o
        thread_info.write_mons[work_id].block_number = count;
        thread_info.write_mons[work_id].buffer_addr = &write_buf;
        thread_info.write_mons[work_id].request_time = clock;
        write_circ_buf(thread_info.write_queues[d], work_id);
        //increment time
        clock = clock + 1;
        //call writer
    }

    //release write lock on file o
    pthread_mutex_unlock(&thread_info.write_mons[work_id].lock);
    //release read lock on file i
    pthread_mutex_unlock(&thread_info.read_mons[work_id].lock);

}
