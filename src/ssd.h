#include <stdio.h>
#include <stdint.h>

#ifndef __SSD_H__
#define __SSD_H__

#define PAGE_SIZE 4 * 1024 // Byte
#define BLOCK_SIZE 512 * (PAGE_SIZE) // 512 page // Byte


#define PAGE_NUM 512
#define BLOCK_NUM 65536

#define SSD_SIZE (PHYSIZE)

#define STREAM_NUM 16

#define OFFSET_STREAM_0TH 0
#define OFFSET_STREAM_1ST 16 * 1024
#define OFFSET_STREAM_2ND 16 * 1024
#define OFFSET_STREAM_3RD 16 * 1024
#define OFFSET_STREAM_4TH 16 * 1024


typedef struct block {
        int stream_id;
        int offset;
        int flag_op; // open : 1, closed 0,

        int* LBA;

        int *page_bitmap;  // status
        int invalid_page_num;
        int erase_count;
} block_t;


typedef struct log {
        int64_t traff_erase;
        int64_t traff_valid_copy;
        int64_t segment_num;

        int64_t valid_copy_num;
} log_t;

typedef struct reward_contr{
        
} rewd_contr_t;

typedef struct ssd {
        block_t **block;
        int       idx_block;
        
        block_t **block_op;
        int      *idx_block_op;
        // traffic for WAF
        int64_t traff_client;
        int64_t traff_ftl;
        int64_t traff_GC;

        log_t **log_group;

        int flag_GC;
        int flag_GC_re;

        int cnt_block_filled;
} ssd_t;



double get_WAF (ssd_t* my_ssd);
void show_stream_group_log(ssd_t* my_ssd);

#endif