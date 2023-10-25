
#include "queue.h"

_queue* q_init () {
        _queue* q = (_queue*)malloc(sizeof(_queue));

        q->head = 0;
        q->tail = 0;
        q->size = 0;

        
        q->block = (block_t**)malloc(sizeof(block_t**) * QUEUE_SIZE);

        int i;
        for (i = 0; i < QUEUE_SIZE; i++) {
                q->block[i] = (block_t*)malloc(sizeof(block_t));
        }

        return q;
}

_queue* q_push (_queue* q, block_t* block_addr) {
        if (q->size >= QUEUE_SIZE) {
                printf("Queue is full\n");
                return q;
        }

        q->block[q->tail] = block_addr;
        //printf("block invalid page num : %d\n", block_addr->invalid_page_num);
        q->tail = (q->tail + 1) % (QUEUE_SIZE);
        q->size++;
        return q;
}       

block_t* q_pop (_queue* q) {
        if (q->size == 0) {
                printf("Queue is empty\n");
                return (block_t*)NULL;
        }
        block_t* block_addr = q->block[q->head];
        
        q->head = (q->head + 1) % (QUEUE_SIZE);


        q->size--;
        return block_addr;
}

_queue* q_destroy (_queue* q) {
        int i;
        for (i = 0; i < QUEUE_SIZE; i++) {
                free(q->block[i]);
        }
        free(q);
} 
