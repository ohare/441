#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "worker.h"
#include "disc.h"
#include "tools.h"
 
//Implements the master thread
int main(int argc, char *argv[]){
    if(argc < 4){
        printf("Not enough arguments:%d\n",argc);
        exit(EXIT_FAILURE); 
    }
    //Number of disc drives (1 <= D <= 16)
    int const D = atoi(argv[1]);    
    //Number of worker threads (1 <= W <= 16)
    int const W = atoi(argv[2]);
    //Number of iterations each worker does(1 <= L)
    int const L = atoi(argv[3]);

    /* Debugging */
    /*
    printf("D:%d",D);
    printf("\nW:%d",W);
    printf("\nL:%d\n",L);
    */

    info thread_info;

    /*Initialise mutexs */
    thread_info.read_mons = emalloc(sizeof(pthread_mutex_t) * D);
    thread_info.write_mons = emalloc(sizeof(pthread_mutex_t) * D);
    thread_info.read_resp_lock = emalloc(sizeof(pthread_mutex_t) * W);
    thread_info.write_resp_lock = emalloc(sizeof(pthread_mutex_t) * W);
    /* and conditions */
    thread_info.read_resp_fin = emalloc(sizeof(pthread_cond_t) * W);
    thread_info.write_resp_fin = emalloc(sizeof(pthread_cond_t) * W);
    thread_info.read_ready = emalloc(sizeof(pthread_cond_t) * D);
    thread_info.write_ready = emalloc(sizeof(pthread_cond_t) * D);

    /* Initialise array of read/write queues for disc */
    thread_info.read_queues = emalloc(sizeof(circ_buf) * D);
    thread_info.write_queues = emalloc(sizeof(circ_buf) * D);
    /* Initialise array of read/write response monitors for workers */
    mon *r_resp = emalloc(sizeof(mon) * W);
    mon *w_resp = emalloc(sizeof(mon) * W);
    thread_info.read_response = r_resp;
    thread_info.write_response = w_resp;
    //thread_info.read_response = emalloc(sizeof(mon) * W);
    //thread_info.write_response = emalloc(sizeof(mon) * W);

    /* Initialise array of ids */
    thread_info.disc_ids = emalloc(sizeof(int) * D);
    thread_info.work_ids = emalloc(sizeof(int) * W);

    /* Initialise array of disc_kill messages */
    thread_info.disc_kill = emalloc(sizeof(int) * D);
    /* Initialise arrays for clocks */
    thread_info.disc_times = emalloc(sizeof(int) * D);
    thread_info.work_times = emalloc(sizeof(int) * W);

    /* Store number of discs, workers and iterations */
    thread_info.D = D;
    thread_info.W = W;
    thread_info.L = L;

    pthread_t disc_threads[D], worker_threads[W];
    int rc, i;

    //Set PTHREAD_MUTEX_ERRORCHECK
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
    //Set up conditional attr
    pthread_condattr_t cond_attr;
    if (pthread_condattr_init(&cond_attr) != 0){
        printf("\nMaster: Cond_attr init failed\n");
    }

    //starts D disc threads
    for (i=0; i < D; ++i) {
        /* Initialise read-write locks */
        if (pthread_mutex_init(&(thread_info.read_mons[i]), &mutex_attr) != 0){
            printf("\nMaster: %d, disc read mutex init failed\n",i);
        }
        if (pthread_mutex_init(&(thread_info.write_mons[i]), &mutex_attr) != 0){
            printf("\nMaster: %d, disc write mutex init failed\n",i);
        }
        if (pthread_cond_init(&(thread_info.read_ready[i]), &cond_attr) != 0){
            printf("\nMaster: %d, worker read response cond init failed\n",i);
        }
        if (pthread_cond_init(&(thread_info.write_ready[i]), &cond_attr) != 0){
            printf("\nMaster: %d, worker write response cond init failed\n",i);
        }
        thread_info.read_queues[i].head = 0;
        thread_info.read_queues[i].tail = 0;
        thread_info.write_queues[i].head = 0;
        thread_info.write_queues[i].tail = 0;
        thread_info.disc_times[i] = 0;
        printf("Creating disc thread %d\n", i);
        rc = pthread_create(&disc_threads[i], NULL, disc_start, &thread_info);
        if(rc != 0){
            printf("Creation of disc thread:%d failed, code:%d\n",i,rc);
            exit(EXIT_FAILURE);
        }
        //printf("Creation of disc thread:%u successful\n",disc_threads[i]);
        thread_info.disc_ids[i] = disc_threads[i];
    }

    //starts W worker threads
    for (i=0; i < W; ++i) {
        if (pthread_mutex_init(&(thread_info.read_resp_lock[i]), &mutex_attr) != 0){
            printf("\nMaster: %d, worker read response mutex init failed\n",i);
        }
        if (pthread_mutex_init(&(thread_info.write_resp_lock[i]), &mutex_attr) != 0){
            printf("\nMaster: %d, worker write response mutex init failed\n",i);
        }
        if (pthread_cond_init(&(thread_info.read_resp_fin[i]), &cond_attr) != 0){
            printf("\nMaster: %d, worker read response cond init failed\n",i);
        }
        if (pthread_cond_init(&(thread_info.write_resp_fin[i]), &cond_attr) != 0){
            printf("\nMaster: %d, worker write response cond init failed\n",i);
        }
        printf("Creating worker thread %d\n", i);
        rc = pthread_create(&worker_threads[i], NULL, work, &thread_info);
        if(rc != 0){
            printf("Creation of worker thread:%d failed, code:%d\n",i,rc);
            exit(EXIT_FAILURE);
        }
        thread_info.work_ids[i] = worker_threads[i];
    }

    //waits for worker threads to complete
    for(i=0; i < W; i++){
        rc = pthread_join(worker_threads[i], NULL);
        //printf("Return code of worker thread:%d was:%d\n",i,rc);
        //Reports how long each worker took
        printf("Worker:%d finished clock:%d\n",i,thread_info.work_times[i]);
    }

    for(i=0; i < D; i++){
        thread_info.disc_kill[i] = 1;
    }

    //waits for disc threads to complete
    for(i=0; i < D; i++){
       rc = pthread_join(disc_threads[i], NULL);
       //printf("Return code of disc thread:%d was:%d\n",i,rc);
    }

    exit(EXIT_SUCCESS); 
}
