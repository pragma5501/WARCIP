#ifndef __FTL_H__
#define __FTL_H__


#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include "queue.h"
#include "WARCIP.h"
#include "dispatcher.h"

#define M_TABLE_SIZE 3125 * 100 * 100
#define THRESHOLD_FREE_Q 100

typedef struct __req_FTL_t {
        double time;
        int type;
        int LBA;
        int size;
        int stream_id;

        w_driver_t* w_driver;
        w_dispatcher_t* w_dispatcher;
} req_FTL_t;

enum IO {
        ERASED = 1, // Erased status
        VALID = 0, // Valid  status
        INVALID = 2,

        GC_T = 0,
        GC_F = 1,

        OPEN = 1,
        CLOSED = 0,
};

#endif


ssd_t* ssd_t_init ();
ssd_t* block_t_init (ssd_t* my_ssd);
void page_init (block_t* my_block);
void log_t_init(ssd_t* my_ssd);
int* page_erase (int* page);

ssd_t* ssd_t_write (ssd_t* my_ssd, int PPN, int page_bit, int LBA);

void destroy_ssd (ssd_t* my_ssd);

_queue* free_q_init (ssd_t* my_ssd, _queue* q);
int free_q_pop (ssd_t* my_ssd,_queue* free_q, req_FTL_t* req_FTL);
void init_mapping_table ();
double get_utilization ();

ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, req_FTL_t* req_FTL);

// Garbage Colloection
void GC_trigger(ssd_t* my_ssd, _queue* free_q, req_FTL_t* req_FTL);
int GC (ssd_t* my_ssd, _queue* free_q, req_FTL_t* req_FTL);
int get_victim (ssd_t* my_ssd);