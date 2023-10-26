#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/kernel.h>
#include <unistd.h>
#include <stdint.h>
#include <regex.h>

#include "FTL.h"
#include "WARCIP.h"
#include "dispatcher.h"
#define BUFF_SIZE 1024

#define DEBUG 1
//#define DEBUG_PRINT 1

enum TYPE {
        READ  = 0,
        WRITE = 1,
        TRIM  = 3,                
};

long long progress; 
int GB;

int stream_log[STREAM_NUM];

w_driver_t* w_driver;
w_dispatcher_t* my_dispatcher;
req_FTL_t* req;

int parse (char *text, ssd_t* my_ssd, _queue* free_q) {

        double waf;
        double tmp_waf = 0;
        double utilization;

        if (sscanf(text, "%lf %d %d %d %d", &req->time, &req->type, 
                                &req->LBA, &req->size, &req->stream_id) == 5) {
                #ifdef DEBUG_PRINT

                #endif
        }

        double RWI = get_RWI(w_driver, req->LBA, req->time);
        update_time_table(w_driver, req->LBA, req->time);

        int cluster_id = WARCIP(w_driver, RWI);

        req->stream_id = dispatch(my_dispatcher, w_driver->clusters[cluster_id].stream_id);
        w_driver->clusters[cluster_id].stream_id = req->stream_id;
        

        stream_log[req->stream_id]++;
        
        switch (req->type) {
        // read
        case READ:
                break;        

        // write
        case WRITE:
                #ifdef DEBUG
                my_ssd = trans_IO_to_ssd(my_ssd, free_q, req);
                w_driver->cnt_block_filled = my_ssd->cnt_block_filled;

                if ((++progress) % (262144 * 1) == 0) {
                        GB += 1;
                        waf = get_WAF(my_ssd);
                        utilization = get_utilization();

                        printf("[Progress %d GB] : WAF : %.2f, TMP_WAF: %.2f, Utiliztion: %.2f\n", GB, waf, tmp_waf, utilization);

                        show_stream_group_log(my_ssd);
                }
                if ((progress) % (262144 * 50) == 0) {
                        
                        tmp_waf = get_WAF(my_ssd);
                }
                #endif
                break;
                
        // trim
        case TRIM:
                break;
        
        default:
                printf("Error : request type is undefined!!\n");
                break;
        }
        
        // WARCIP reward process
        if (my_ssd->log_group[req->stream_id]->valid_copy_num > 0) {
                w_driver->clusters[cluster_id].num_pages = my_ssd->log_group[req->stream_id]->valid_copy_num;   
                w_driver->clusters[cluster_id].center *= 2;           
                my_ssd->log_group[req->stream_id]->valid_copy_num = 0;
                
                sort_clusters(w_driver);
        } 
        
}

int read_request (FILE* fp, ssd_t* my_ssd, _queue* free_q) 
{
        char buf[BUFF_SIZE];

        while (fgets(buf, sizeof(buf), fp)) {
                parse(buf, my_ssd, free_q);
        }
}

int main (int argc, char** argv) 
{
        // initialize ssd
        ssd_t* my_ssd = ssd_t_init();
        _queue* free_q = free_q_init(my_ssd, q_init());

        

        // initialze mapping table by set value of mapping table -1
        init_mapping_table();
        
        w_driver = w_driver_t_init(w_driver);
        my_dispatcher = w_dispatcher_t_init(my_dispatcher);

        // put WARCIP ENGINE pointer into FTL request.
        req = (req_FTL_t*)malloc(sizeof(req_FTL_t));
        req->w_driver = w_driver;
        req->w_dispatcher = my_dispatcher;
        
        w_driver->sender_dispatcher = my_dispatcher;

        FILE* fp = fopen("./src/intern-trace", "r");
        if( fp == NULL ) {
                printf("Error : File not opened\n");
                return 0;
        }
        progress = 0;
        GB = 0;
        read_request(fp, my_ssd, free_q);
        fclose(fp);

        printf("User Write : %ld\n", my_ssd->traff_client);
        printf("GC Write   : %ld\n", my_ssd->traff_GC);
        printf("free q size : %d\n", free_q->size);

        destroy_ssd(my_ssd);
        q_destroy(free_q);

        return 0;
}


