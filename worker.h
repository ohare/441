#ifndef WORKER_H_
#define WORKER_H_

void *work(void *L);
void read_file(void* thread_info, int i, int work_id);
void write_file(void* thread_info, int i, int work_id);

#endif
