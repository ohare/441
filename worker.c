#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "tools.h"
#include "worker.h"
#include "assert.h"
#include "time.h"

#define BUF_SIZE 4096

#define BLOCK_SIZE_IN_KIB 4
#define BLOCK_SIZE 4096
/* Had to reduce blocks per file as program was running too slowly */
#define BLOCKS_PER_FILE 250

void *work(void *args){
    int i = 0;
    int o = 0;
    //int d = 0;
    int F = 1000;
    int count = 0;
    //int blocks_per_file = 250;
    //int work_id = *((int *) L);
    int work_id = 0;
    //info thread_info = *((info *)(args));
    info thread_info = *((info *)(args));
    //char read_buf[BUF_SIZE];
    //char write_buf[BUF_SIZE];
    //Worker w starts w seconds after origin
    thread_info.work_times[work_id] = work_id;

    work_id = get_id(&thread_info, pthread_self());

    printf("I am worker %d\n",work_id);
        
    /* For L iterations */
    for(count = 0; count < thread_info.L; count++){
        while(i == o){
            i = F * drand48();
            o = F * drand48();
        }

        printf("Input file:%d\n",i);
        printf("Output file:%d\n",o);

    /* TODO implement file locks? Don't really need this?
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
        read_file(&thread_info,i,work_id);
        //write at block 0 of file o
        //write_file(&thread_info,o,d,count,work_id, write_buf);

    /*
        //release write lock on file o
        pthread_mutex_unlock(&thread_info.write_mons[work_id].lock);
        //release read lock on file i
        pthread_mutex_unlock(&thread_info.read_mons[work_id].lock);
    */
    }

    return 0;
}

void read_file(void* thread_info, int i, int work_id){
    /* Read a 1 kiB record from input file i */
    info *ti = (info *)(thread_info);
    char* read_buffers[BUF_SIZE];
    int d = 0;
    long x;
    int block_num;
    int block_on_disc;

    for(block_num = 0; block_num < BLOCKS_PER_FILE; block_num++){
        x = (long) i * BLOCKS_PER_FILE + block_num;
        d = x % ti->D;
        block_on_disc = x / ti->D;
        /* Lock read queue */
        pthread_mutex_lock(&ti->read_mons[d]);

        /* Write monitor to queue if there is space */
        //pthread_mutex_unlock(&ti->read_mons[d]);
        for(;;){
            //pthread_mutex_lock(&ti->read_mons[d]);
            if(!is_circ_full(&ti->read_queues[d])){
                write_circ_buf(&ti->read_queues[d], block_on_disc, *read_buffers, ti->work_times[work_id], work_id, 0);
                break;
            }
            //pthread_mutex_unlock(&ti->read_mons[d]);
        }

        /* Unlock queue */
        pthread_mutex_unlock(&ti->read_mons[d]);

        /* Get back completion time from disc */
        for(;;){
            pthread_mutex_lock(&ti->read_resp_lock[work_id]);
            pthread_cond_wait(&ti->read_resp_fin[work_id],&ti->read_resp_lock[work_id]);
            //if(ti->read_response[work_id].finished == 1){
                //printf("Worker finished!\n");
            ti->work_times[work_id] = ti->read_response[work_id].completion_time;
                //printf("Time now%d\n", ti->work_times[work_id]);
            ti->read_response[work_id].finished = 0;
            break;
            //}
            pthread_mutex_unlock(&ti->read_resp_lock[work_id]);
        }
    }
}

void write_file(void* thread_info, int o, int work_id){
    /* Read a 1 kiB record from input file i */
    info *ti = (info *)(thread_info);
    char* write_buffers[BUF_SIZE];
    int d = 0;
    long x;
    int block_num;
    int block_on_disc;

    for(block_num = 0; block_num < BLOCKS_PER_FILE; block_num++){
        x = (long) o * BLOCKS_PER_FILE + block_num;
        d = x % ti->D;
        block_on_disc = x / ti->D;
        /* Lock read queue */
        pthread_mutex_lock(&ti->write_mons[d]);

        /* Write monitor to queue if there is space */
        //pthread_mutex_unlock(&ti->read_mons[d]);
        for(;;){
            //pthread_mutex_lock(&ti->read_mons[d]);
            if(!is_circ_full(&ti->write_queues[d])){
                write_circ_buf(&ti->write_queues[d], block_on_disc, *write_buffers, ti->work_times[work_id], work_id, 0);
                break;
            }
            //pthread_mutex_unlock(&ti->read_mons[d]);
        }

        /* Unlock queue */
        pthread_mutex_unlock(&ti->write_mons[d]);

        /* Get back completion time from disc */
        for(;;){
            pthread_mutex_lock(&ti->write_resp_lock[work_id]);
            pthread_cond_wait(&ti->write_resp_fin[work_id],&ti->write_resp_lock[work_id]);
            //if(ti->read_response[work_id].finished == 1){
                //printf("Worker finished!\n");
            ti->work_times[work_id] = ti->write_response[work_id].completion_time;
                //printf("Time now%d\n", ti->work_times[work_id]);
            ti->write_response[work_id].finished = 0;
            break;
            //}
            pthread_mutex_unlock(&ti->write_resp_lock[work_id]);
        }
    }
}
/*
void write_file(void* thread_info,int i, int d, int count, int work_id, char* write_buf){
    //write that 1 kiB record to output file o
    info ti = *((info *)(thread_info));
    pthread_mutex_lock(&ti.write_mons[d]);

    mon temp;

    temp.block_number = count;
    temp.buffer_addr = write_buf;
    temp.request_time = ti.work_times[work_id];
    temp.work_id = work_id;
    temp.finished = 0;

    //increment time by 1 before write
    ti.work_times[work_id] = ti.work_times[work_id] + 1;

    write_circ_buf(&ti.write_queues[d], &temp);
    //printf("Reciept time:%d\n",ti.write_mons[d].completion_time);
    pthread_mutex_unlock(&ti.write_mons[d]);

    //call writer
}
*/
