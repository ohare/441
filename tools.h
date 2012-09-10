#ifndef TOOLS_H_
#define TOOLS_H_

#define BUFFER_SIZE 256                    /* Size of the circular buffer */

typedef struct circular_buffer{
    int content[BUFFER_SIZE];
    int head;
    int tail;
} circ_buf;

void write_circ_buf(circ_buf c, int data);
int read_circ_buf(circ_buf c);
int is_circ_empty(circ_buf c);
void* emalloc(size_t s);

#endif
