#ifndef TOOLS_H_
#define TOOLS_H_

#define BUFFER_SIZE 256                    /* Size of the circular buffer */

typedef struct monitor{
    pthread_mutex_t lock;
    int block_number;
    int request_time;
    int receipt_time;
    int completion_time;
    int work_id;
    int finished;
    void* buffer_addr;
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
    int head;
    int tail;
    pthread_mutex_t lock;
    mon *content[BUFFER_SIZE];
} circ_buf;

typedef struct worker_t{
    mon read_resp;
    mon write_resp;
    int time;
} worker;

typedef struct info_t{
    int D;
    int W;
    int L;
    pthread_t *disc_ids;
    pthread_t *work_ids;
    mon *read_response;
    mon *write_response;
    pthread_mutex_t *read_mons;
    pthread_mutex_t *write_mons;
    pthread_mutex_t *read_resp_lock;
    pthread_cond_t *read_resp_fin;
    pthread_mutex_t *write_resp_lock;
    pthread_cond_t *write_resp_fin;
    pthread_mutex_t *file_read_lock;
    pthread_mutex_t *file_write_lock;
    int *disc_kill;
    int *disc_times;
    int *work_times;
    worker *workers;
    circ_buf *read_queues;
    circ_buf *write_queues;
} info;

//void write_circ_buf(circ_buf *c, mon *monitor);
void write_circ_buf(circ_buf *c, int count, char* write_buf, int req_time, int work_id, int finished);
mon *read_circ_buf(circ_buf *c);
int is_circ_empty(circ_buf *c);
int is_circ_full(circ_buf *c);
void* emalloc(size_t s);
int get_id(info *i, pthread_t self_id);

#endif
