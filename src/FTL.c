#include "FTL.h"
#include "err.h"

int mapping_table[M_TABLE_SIZE];



ssd_t* ssd_t_init () 
{
        ssd_t* my_ssd = (ssd_t*)malloc(sizeof(ssd_t));

        // malloc error handling
        my_assert(handle_allc_err(my_ssd));

        block_t_init(my_ssd);

        my_ssd->traff_client = 0;
        my_ssd->traff_ftl    = 0;
        my_ssd->traff_GC     = 0;

        log_t_init(my_ssd);

        my_ssd->flag_GC      = GC_F;
        my_ssd->flag_GC_re   = GC_F;

        my_ssd->cnt_block_filled = 0;

        return my_ssd;
}

ssd_t* block_t_init (ssd_t* my_ssd) 
{
        block_t** my_block = (block_t**)malloc(sizeof(block_t*) * BLOCK_NUM);
        my_assert(handle_allc_err(my_block));

        int i;
        for (i = 0; i < BLOCK_NUM; i++) {

                my_block[i] = (block_t*)malloc(sizeof(block_t));
                my_assert(handle_allc_err(my_block[i])); 

                my_block[i]->stream_id = -1;

                // init page info
                my_block[i]->offset = i * (PAGE_NUM);
                my_block[i]->invalid_page_num = 0;

                page_init(my_block[i]);
        }
        
        block_t** block_op = (block_t**)malloc(sizeof(block_t*));
        my_assert(handle_allc_err(block_op));

        for (i = 0; i < STREAM_NUM; i++) {
              block_op[i] = (block_t*)malloc(sizeof(block_t));  
              my_assert(handle_allc_err(block_op[i]));
        }

        int *idx_block_op = (int*)malloc(sizeof(int) * STREAM_NUM);
        my_assert(handle_allc_err(idx_block_op));

        for (i = 0; i < STREAM_NUM; i++) {
                idx_block_op[i] = 512;
        }

        

        my_ssd->block         = my_block;
        my_ssd->block_op      = block_op;
        my_ssd->idx_block_op  = idx_block_op;



        return my_ssd;
}


void page_init (block_t* my_block) 
{
        my_block->LBA = (int*)malloc(sizeof(int) * PAGE_NUM);
        my_block->page_bitmap = (int*)malloc(sizeof(int) * PAGE_NUM);

        // malloc error handling
        my_assert(handle_allc_err(my_block->LBA));
        my_assert(handle_allc_err(my_block->page_bitmap)); 

        int i;
        for (i = 0; i < PAGE_NUM; i++) {
                my_block->page_bitmap[i] = ERASED;
        }

}

void log_t_init(ssd_t* my_ssd) {
        log_t **log = (log_t**)malloc(sizeof(log_t*) * STREAM_NUM);
        my_assert(handle_allc_err(log));

        int i;
        for (i = 0; i < STREAM_NUM; i++) {
                log[i] = (log_t*)malloc(sizeof(log_t));
                log[i]->traff_erase = 0;
                log[i]->traff_valid_copy = 0;
                log[i]->segment_num = 0;

                log[i]->valid_copy_num = 0;
        }

        my_ssd->log_group = log;
}

int* page_erase (int* page) 
{
        int i;
        for(i = 0; i < PAGE_NUM; i++) {
                page[i] = ERASED;
        }
        return page;
}


ssd_t* ssd_t_write (ssd_t* my_ssd, int PPN, int page_bit, int LBA) 
{
        // get the position of page from PBN
        int block_n = (int)(PPN / (PAGE_NUM));
        int page_n = PPN % (PAGE_NUM);
        //printf("block offset : %d\n", block_n);
        // just write
        my_ssd->block[block_n]->page_bitmap[page_n] = page_bit;
        my_ssd->block[block_n]->LBA[page_n] = LBA;

        // if invalid 
        if (page_bit == INVALID) {
                //printf("invalid block offset : %d\n", block_n);
                my_ssd->block[block_n]->invalid_page_num += 1;

                return my_ssd;
        }

        mapping_table[LBA] = PPN;

        // traffic 
        my_ssd->traff_ftl += 1;

        return my_ssd;
}



void destroy_ssd (ssd_t* my_ssd) 
{
        int i;
        for (i = 0; i < BLOCK_NUM; i++) {
                free(my_ssd->block[i]->page_bitmap);
                free(my_ssd->block[i]->LBA);
                free(my_ssd->block[i]);
        }

        free(my_ssd->block);

        for (i = 0; i < STREAM_NUM; i++) {
                free(my_ssd->block_op[i]);
        }

        free(my_ssd->block_op);
        free(my_ssd->idx_block_op);
        


        
        for (i = 0; i < STREAM_NUM; i++) {
                free(my_ssd->log_group[i]);
        }

        free(my_ssd->log_group);
        free(my_ssd);
}

int get_PPN (ssd_t* my_ssd, int stream_id) 
{
        if (stream_id == -1) {
                printf("fuck\n");
        }
        return my_ssd->idx_block_op[stream_id] + my_ssd->block_op[stream_id]->offset;
}

// free block queue
_queue* free_q_init (ssd_t* my_ssd, _queue* q) 
{
        int i;

        for (i = 0; i < BLOCK_NUM; i++) {
                q_push(q, my_ssd->block[i]);
        }

        printf("queue size : %d\n", q->size);
        return q;
}

int free_q_pop (ssd_t* my_ssd, _queue* free_q, req_FTL_t* req_FTL) 
{
        int stream_id = req_FTL->stream_id;
        
        if (my_ssd->idx_block_op[stream_id] >= PAGE_NUM) {
                
                my_ssd->block_op[stream_id]->flag_op = CLOSED;
                my_ssd->cnt_block_filled++;

                my_ssd->idx_block_op[stream_id] = 0;
                my_ssd->block_op[stream_id] = q_pop(free_q);
                my_ssd->block_op[stream_id]->stream_id = stream_id;
                my_ssd->log_group[stream_id]->segment_num++;


                my_ssd->block_op[stream_id]->flag_op = OPEN;
                my_ssd->idx_block_op[stream_id] += 1;

                
                GC_trigger(my_ssd, free_q, req_FTL);
        }
        
        int PPN = get_PPN(my_ssd, stream_id);
        my_ssd->idx_block_op[stream_id] += 1;

        return PPN;
}



void init_mapping_table () 
{
        int i;
        for (i = 0; i < M_TABLE_SIZE; i++) {
                mapping_table[i] = -1;
        }
}

double get_utilization ()
{      
        int valid_LBA;

        int i;
        for (i = 0; i < M_TABLE_SIZE; i++) {
                if (mapping_table[i] == -1) {
                        continue;
                }
                valid_LBA++;
        }

        return (double)valid_LBA / (double)(M_TABLE_SIZE);
}



ssd_t* trans_IO_to_ssd (ssd_t* my_ssd,_queue* free_q, req_FTL_t* req_FTL) 
{
        int PPN;
        int stream_id = req_FTL->stream_id;
        int LBA = req_FTL->LBA;

        // if modify
        if (mapping_table[LBA] != -1) {
                PPN = mapping_table[LBA];
                my_ssd = ssd_t_write(my_ssd, PPN, INVALID, LBA);
                
        }

        PPN = free_q_pop(my_ssd, free_q, req_FTL);

        //printf("LBA -> PPN : %d -> %d\n", LBA, PPN);
        my_ssd = ssd_t_write(my_ssd, PPN, VALID, LBA);

        my_ssd->traff_client += 1;
        return my_ssd;
}

void GC_trigger(ssd_t* my_ssd, _queue* free_q, req_FTL_t* req_FTL) 
{
        if (my_ssd->flag_GC == GC_T) {
                return;
        }


        while (free_q->size < THRESHOLD_FREE_Q) {
                GC(my_ssd, free_q, req_FTL);
        }
        


}

int GC (ssd_t* my_ssd, _queue* free_q, req_FTL_t* req_FTL) 
{

        my_ssd->flag_GC = GC_T;
        my_ssd->traff_GC++;

        int block_n_victim = get_victim(my_ssd);
        block_t* block_victim = my_ssd->block[block_n_victim];

        
        int stream_id = -1;

        int i;
        for(i = 0; i < PAGE_NUM; i++) {

                int page_bit = block_victim->page_bitmap[i];
                if (page_bit == INVALID) {
                        continue;
                }

                // a page_bit is valid
                if (page_bit != VALID) {
                        continue;
                }
                
                int LBA = block_victim->LBA[i];

                // GC Feedback
                double RWI = get_RWI(req_FTL->w_driver, LBA, req_FTL->time) * 2;
                update_time_table(req_FTL->w_driver, LBA, req_FTL->time);
                
                int cluster_id = WARCIP(req_FTL->w_driver, RWI);

                int tmp_id = req_FTL->w_driver->clusters[cluster_id].stream_id;
                stream_id = dispatch(req_FTL->w_dispatcher, tmp_id);
                req_FTL->stream_id = stream_id;
                req_FTL->w_driver->clusters[cluster_id].stream_id = stream_id;
                // GC Feedback done

                int PPN = free_q_pop(my_ssd, free_q, req_FTL);

                ssd_t_write(my_ssd, PPN, VALID, LBA);

                my_ssd->log_group[stream_id]->valid_copy_num++;
                my_ssd->log_group[stream_id]->traff_valid_copy++;
        }
        //printf("GC result\n");
        //printf("victim block : %d\n", block_n_victim);
        //printf("invalid page num : %d\n", my_ssd->block[block_n_victim]->invalid_page_num);

        stream_id = block_victim->stream_id;

        page_erase(my_ssd->block[block_n_victim]->page_bitmap);
        my_ssd->block[block_n_victim] = block_victim;
        my_ssd->block[block_n_victim]->invalid_page_num = 0;
        my_ssd->block[block_n_victim]->stream_id = -1;

        q_push(free_q, my_ssd->block[block_n_victim]);
        
        my_ssd->log_group[stream_id]->segment_num--;
        my_ssd->log_group[stream_id]->traff_erase++;
        my_ssd->log_group[stream_id]->traff_valid_copy += my_ssd->log_group[stream_id]->valid_copy_num;
        my_ssd->flag_GC = GC_F;



}

// select a block that has most invalid page
int get_victim (ssd_t* my_ssd) 
{

        int max = -1;
        int max_i = 0;
        int i;
        for (i = 0; i < BLOCK_NUM; i++ ) {
                if (my_ssd->block[i]->flag_op != CLOSED) {
                        continue;
                }
                int tmp = my_ssd->block[i]->invalid_page_num;
                int tmp_id = my_ssd->block[i]->stream_id;

                max_i = (tmp > max ) ? i   : max_i;
                max   = (tmp > max ) ? tmp : max;
                
        }
        // printf("invalid page num %d\n", max);
        return max_i;
}


