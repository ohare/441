#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "worker.h"
#include "disc.h"
#include "tools.h"
 
//Implements the master thread
int main(int argc, char *argv[]){
     //Number of disc drives (1 <= D <= 16)
    int const D = atoi(argv[1]);    
    //Number of worker threads (1 <= W <= 16)
    int const W = atoi(argv[2]);
    //Number of iterations each worker does(1 <= L)
    int const L = atoi(argv[3]);

    //creates D queues
    printf("D:%d",D);
    //initialises W reader-writer locks
    printf("\nW:%d",W);

    printf("\nL:%d\n",L);

    //Initialise read and write lock array
    read_locks = emalloc(sizeof(pthread_mutex_t) * D);
    write_locks = emalloc(sizeof(pthread_mutex_t) * D);

    /* Initialise array of read/write queues for disk */
    read_queues = emalloc(sizeof(circ_buf) * D);
    write_queues = emalloc(sizeof(circ_buf) * D);

    /* Initialise array of read/write queues for disk */
    read_queues = emalloc(sizeof(circ_buf) * D);
    write_queues = emalloc(sizeof(circ_buf) * D);

    pthread_t disc_threads[D], worker_threads[W];
    int disc_thread_args[D], worker_thread_args[W];
    int rc, i;

    //starts D disk threads
    for (i=0; i < D; ++i) {
        read_queues[i].head = 0;
        read_queues[i].tail = 0;
        write_queues[i].head = 0;
        write_queues[i].tail = 0;
        if (pthread_mutex_init(&read_locks[i], NULL) != 0){
            printf("\nDisk: %d, read mutex init failed\n",i);
        }

        if (pthread_mutex_init(&write_locks[i], NULL) != 0){
            printf("\nDisk: %d, write mutex init failed\n",i);
        }
        disc_thread_args[i] = i;
        printf("In main: creating disc thread %d\n", i);
        rc = pthread_create(&disc_threads[i], NULL, disc_start, (void *) &disc_thread_args[i]);
        if(rc != 0){
            printf("Creation of disc thread:%d failed, code:%d\n",i,rc);
            exit(EXIT_FAILURE);
        }
    }

    //starts W worker threads
    for (i=0; i < W; ++i) {
       worker_thread_args[i] = i;
       printf("In main: creating worker thread %d\n", i);
       rc = pthread_create(&worker_threads[i], NULL, work, (void *) &worker_thread_args[i]);
       if(rc != 0){
            printf("Creation of worker thread:%d failed, code:%d\n",i,rc);
            exit(EXIT_FAILURE);
       }
    }

    //waits for worker threads to complete
    for (i=0; i < W; ++i) {
       rc = pthread_join(worker_threads[i], NULL);
       printf("Return code of worker thread:%d was:%d\n",i,rc);
    }
 

    //waits for disc threads to complete
    for (i=0; i < D; ++i) {
       rc = pthread_join(disc_threads[i], NULL);
       printf("Return code of disc thread:%d was:%d\n",i,rc);
    }

    //reports how long each worker took

    exit(EXIT_SUCCESS); 
}
