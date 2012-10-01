#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "worker.h"
#include "tools.h"

#define BUF_SIZE 4096

void *work(void *L){
    int i = 0;
    int o = 0;
    int F = 1000;
    int count = 0;
    int rep = 0;
    int clock = 0;
    int work_id = *((int *) L);
    char read_buf[BUF_SIZE];
    char write_buf[BUF_SIZE];

    printf("IamA Worker %d\n",work_id);

    while(i == o){
        i = 1000 * drand48();
        o = 1000 * drand48();
    }

    if(i < o){
        //obtain a read lock on file i
        pthread_mutex_lock(&read_mons[work_id].lock);
        //obtain a write lock on file o
        pthread_mutex_lock(&write_mons[work_id].lock);
    } else {
        //obtain a write lock on file o
        pthread_mutex_lock(&write_mons[work_id].lock);
        //obtain a read lock on file i
        pthread_mutex_lock(&read_mons[work_id].lock);
    }

    //read from block 0 of file i
    //write at block 0 of file o

    for(count = 0; count <= 125000; count++){
        //read a 1 kiB record from input file i
        //write that 1 kiB record to output file o
    }

    //release write lock on file o
    pthread_mutex_unlock(&write_mons[work_id].lock);
    //release read lock on file i
    pthread_mutex_unlock(&read_mons[work_id].lock);

}
