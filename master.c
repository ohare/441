#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "worker.h"
#include "disc.h"
#include "tools.h"
 
//Implements the master thread
int main(void){
    int D; //Number of disc drives (1 <= D <= 16)
    int W; //Number of worker threads (1 <= W <= 16)
    int L; //Number of iterations each worker does(1 <= L)

    //creates D queues
    D = 4;
    //initialises W reader-writer locks
    W = 6;

    /* Initialise circular buffer array */
    circ_buf *circ_buffers;

    //TODO: get emalloc from old project
    circ_buffers = emalloc(sizeof(circ_buf) * D);

    pthread_t disc_threads[D], worker_threads[W];
    int disc_thread_args[D], worker_thread_args[W];
    int rc, i;

    //starts D disk threads
    for (i=0; i < D; ++i) {
       circ_buffers[i].head = 0;
       circ_buffers[i].tail = 0;
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
