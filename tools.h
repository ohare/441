#ifndef TOOLS_H_
#define TOOLS_H_

typedef struct circular_buffer circ_buf;
void write_circ_buf(circ_buf c, int data);
int read_circ_buf(circ_buf c);
int is_circ_empty(circ_buf c);

#endif
