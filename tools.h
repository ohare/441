#ifndef TOOLS_H_
#define TOOLS_H_

#define BUFFER_SIZE 256                    /* Size of the circular buffer */

typedef struct circular_buffer{
    int content[BUFFER_SIZE];
    int head;
    int tail;
} circ_buf;

typedef struct read_monitor{
    pthread_mutex_t lock;
    int block_number;
    void* buffer_addr;
    int request_time;
    int receipt_time;
    int completion_time;
} rm;

typedef struct write_monitor{
    pthread_mutex_t lock;
    int block_number;
    void* buffer_addr;
    int request_time;
    int receipt_time;
    int completion_time;
} wm;

typedef struct info_t{
    pthread_t *disk_ids;
    pthread_t *work_ids;
    circ_buf *read_queues;
    circ_buf *write_queues;
    rm *read_mons;
    wm *write_mons;
    int D;
    int W;
} info;

void write_circ_buf(circ_buf c, int data);
int read_circ_buf(circ_buf c);
int is_circ_empty(circ_buf c);
int is_circ_full(circ_buf c);
void* emalloc(size_t s);
int get_id(info i, pthread_t self_id);

#endif
