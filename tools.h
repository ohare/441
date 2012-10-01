#ifndef TOOLS_H_
#define TOOLS_H_

#define BUFFER_SIZE 256                    /* Size of the circular buffer */

typedef struct circular_buffer{
    int content[BUFFER_SIZE];
    int head;
    int tail;
} circ_buf;

typedef struct read_monitor{
    int block_number;
    void* buffer_addr;
    int request_time;
    int receipt_time;
    int completion_time;
} rm;

typedef struct write_monitor{
    int block_number;
    void* buffer_addr;
    int request_time;
    int receipt_time;
    int completion_time;
} wm;

void write_circ_buf(circ_buf c, int data);
int read_circ_buf(circ_buf c);
int is_circ_empty(circ_buf c);
int is_circ_full(circ_buf c);
void* emalloc(size_t s);
pthread_mutex_t *read_locks;
pthread_mutex_t *write_locks;
circ_buf *read_queues;
circ_buf *write_queues;

#endif
