#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ssd.h"

#define QUEUE_SIZE 4 * 16 * 1024

typedef struct _q {
        int head;
        int tail;
        int size;
        block_t** block;
} _queue;

_queue* q_init();
_queue* q_push (_queue* q, block_t* block_addr);
block_t* q_pop(_queue* q);
_queue* q_destroy(_queue* q);