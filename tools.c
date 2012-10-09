#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "tools.h"

/*
 * Writes to the circular buffer
 * If the buffer is full the first number will be overwritten
 */
void write_circ_buf(circ_buf *c, int data){

    c->content[c->tail] = data;
    c->tail++;
    if(c->tail == c->head){
        c->head++;
        if(c->head == BUFFER_SIZE){
            c->head = 0;
        }
    }
    if(c->tail == BUFFER_SIZE){
        c->tail = 0;
    }
    printf("circ buf wrote:%d, head:%d, tail:%d\n",data,c->head,c->tail);
}

/*
 * Reads from the circular buffer
 */
int read_circ_buf(circ_buf *c){
    int read;

    if(c->head == c->tail){
        return '\0';
    }

    read = c->content[c->head];
    c->head++;
    if(c->head == BUFFER_SIZE){
        c->head = 0;
    }

    printf("circ buf read:%d, head:%d, tail:%d\n",read,c->head,c->tail);

    return read;
}

/*
 * Return whether circular buffer is empty or not
 */
int is_circ_empty(circ_buf *c){
    if(c->head == c->tail){
        //printf("Circ buffer is empty\n");
        return 1;
    }

    printf("Circ buffer is not empty\n");
    return 0;
}

/*
 * Return whether circular buffer is full or not
 */
int is_circ_full(circ_buf *c){
    if(c->head == c->tail - 1){
        return 1;
    }
    if(c->head == 0 && c->tail == BUFFER_SIZE){
        return 1;
    }

    return 0;
}

/*
 * Malloc the given space report error if failure
 */
void *emalloc(size_t s){
    void *result = malloc(s);
    if(result == NULL){
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    return result;
}

int get_id(info i, pthread_t self_id){
    int n = 0;

    //Check lists of disc and worker thread ids to get the 'number' of the thread
    for(;;){
        for(n = 0; n < i.D; n++){
            if(pthread_equal(i.disc_ids[n],self_id)){
                return n;
            }
        }

        for(n = 0; n < i.W; n++){
            if(pthread_equal(i.work_ids[n],self_id)){
                return n;
            }
        }
    }

    return -1;
}
