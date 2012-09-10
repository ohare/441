#include <stdlib.h>
#include <stdio.h>
#include "tools.h"

/*
 * Writes to the circular buffer
 * If the buffer is full the first number will be overwritten
 */
void write_circ_buf(circ_buf c, int data){

    c.content[c.tail] = data;
    c.tail++;
    if(c.tail == c.head){
        c.head++;
        if(c.head == BUFFER_SIZE){
            c.head = 0;
        }
    }
    if(c.tail == BUFFER_SIZE){
        c.tail = 0;
    }
}

/*
 * Reads from the circular buffer
 */
int read_circ_buf(circ_buf c){
    int read;

    if(c.head == c.tail){
        return '\0';
    }

    read = c.content[c.head];
    c.head++;
    if(c.head == BUFFER_SIZE){
        c.head = 0;
    }

    return read;
}

/*
 * Return whether circular buffer is empty or not
 */
int is_circ_empty(circ_buf c){
    if(c.head == c.tail){
        return 1;
    }

    return 0;
}

void *emalloc(size_t s){
    void *result = malloc(s);
    if(result == NULL){
        printf("Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    return result;
}
