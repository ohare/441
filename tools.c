#include "tools.h"

#define BUFFER_SIZE 256                    /* Size of the circular buffer */

typedef struct circular_buffer{
    int content[BUFFER_SIZE];
    int head;
    int tail;
} circ_buf;

circ_buf cbuf;
/* Initialise circular buffer */
cbuf.head = 0;
cbuf.tail = 0;

/*
 * Writes to the circular buffer
 * If the buffer is full the first number will be overwritten
 */
void write_circ_buf(int data){

    cbuf.content[cbuf.tail] = data;
    cbuf.tail++;
    if(cbuf.tail == cbuf.head){
        cbuf.head++;
        if(cbuf.head == BUFFER_SIZE){
            cbuf.head = 0;
        }
    }
    if(cbuf.tail == BUFFER_SIZE){
        cbuf.tail = 0;
    }
}

/*
 * Reads from the circular buffer
 */
int read_circ_buf(void){
    int read;

    if(cbuf.head == cbuf.tail){
        return '\0';
    }

    read = cbuf.content[cbuf.head];
    cbuf.head++;
    if(cbuf.head == BUFFER_SIZE){
        cbuf.head = 0;
    }

    return read;
}

/*
 * Return whether circular buffer is empty or not
 */
int is_circ_empty(void){
    if(cbuf.head == cbuf.tail){
        return 1;
    }

    return 0;
}

