#ifndef TOOLS_H_
#define TOOLS_H_

#define BUFFER_SIZE 256                    /* Size of the circular buffer */

typedef struct monitor{
    pthread_mutex_t lock;
    int block_number;
    void* buffer_addr;
    int request_time;
    int receipt_time;
    int completion_time;
    int work_id;
} mon;

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

//Used as queue for disc requests
typedef struct circular_buffer{
    mon *content[BUFFER_SIZE];
    int head;
    int tail;
    pthread_mutex_t lock;
} circ_buf;

typedef struct info_t{
    pthread_t *disc_ids;
    pthread_t *work_ids;
    circ_buf *read_queues;
    circ_buf *write_queues;
    circ_buf *read_response;
    circ_buf *write_response;
    pthread_mutex_t *read_mons;
    pthread_mutex_t *write_mons;
    pthread_mutex_t *read_resp_lock;
    pthread_mutex_t *write_resp_lock;
    pthread_mutex_t *file_read_lock;
    pthread_mutex_t *file_write_lock;
    int *disc_kill;
    int *disc_times;
    int *work_times;
    int D;
    int W;
    int L;
} info;

void write_circ_buf(circ_buf *c, mon *monitor);
mon *read_circ_buf(circ_buf *c);
int is_circ_empty(circ_buf *c);
int is_circ_full(circ_buf *c);
void* emalloc(size_t s);
int get_id(info *i, pthread_t self_id);

#endif
