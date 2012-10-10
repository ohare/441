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
    printf("D:%d",D);
    printf("\nW:%d",W);
    printf("\nL:%d\n",L);

    info thread_info;

    /* Initialises W reader-writer locks */
    thread_info.read_mons = emalloc(sizeof(rm) * W);
    thread_info.write_mons = emalloc(sizeof(wm) * W);

    /* Initialise array of read/write queues for disc */
    thread_info.read_queues = emalloc(sizeof(circ_buf) * D);
    thread_info.write_queues = emalloc(sizeof(circ_buf) * D);

    /* Initialise array of ids */
    thread_info.disc_ids = emalloc(sizeof(int) * D);
    thread_info.work_ids = emalloc(sizeof(int) * W);

    /* Initialise array of disc_kill messages */
    thread_info.disc_kill = emalloc(sizeof(int) * D);

    /* Store number of discs and workers */
    thread_info.D = D;
    //printf("(master) Set number of discs to:%d\n",thread_info.D);
    thread_info.W = W;

    pthread_t disc_threads[D], worker_threads[W];
    //int disc_thread_args[D], worker_thread_args[W];
    int rc, i;

    //Set PTHREAD_MUTEX_ERRORCHECK
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);

    //starts D disc threads
    for (i=0; i < D; ++i) {
        thread_info.read_queues[i].head = 0;
        thread_info.read_queues[i].tail = 0;
        thread_info.write_queues[i].head = 0;
        thread_info.write_queues[i].tail = 0;
        //disc_thread_args[i] = i;
        printf("Creating disc thread %d\n", i);
        //rc = pthread_create(&disc_threads[i], NULL, disc_start, (void *) &disc_thread_args[i]);
        //printf("(master) info addr:%d\n",&thread_info);
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
        /* Initialise read-write locks */
        if (pthread_mutex_init(&thread_info.read_mons[i].lock, &mutex_attr) != 0){
            printf("\nWorker: %d, read mutex init failed\n",i);
        }
        if (pthread_mutex_init(&thread_info.write_mons[i].lock, &mutex_attr) != 0){
            printf("\nWorker: %d, write mutex init failed\n",i);
        }
        //worker_thread_args[i] = i;
        printf("Creating worker thread %d\n", i);
        //rc = pthread_create(&worker_threads[i], NULL, work, (void *) &worker_thread_args[i]);
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
       printf("Return code of worker thread:%d was:%d\n",i,rc);
    }

    for(i=0; i < D; i++){
        thread_info.disc_kill[i] = 1;
    }

    //waits for disc threads to complete
    for(i=0; i < D; i++){
       rc = pthread_join(disc_threads[i], NULL);
       printf("Return code of disc thread:%d was:%d\n",i,rc);
    }

    //reports how long each worker took

    exit(EXIT_SUCCESS); 
}
