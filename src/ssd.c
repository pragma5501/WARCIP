#include <stdio.h>
#include "ssd.h"

double get_WAF (ssd_t* my_ssd) 
{
        return (double)my_ssd->traff_ftl / (double)my_ssd->traff_client;
}

void show_stream_group_log(ssd_t* my_ssd) 
{

        
        int i;
        for (i = 0; i < STREAM_NUM; i++) {
                double ratio;
                if (my_ssd->log_group[i]->traff_erase > 0) {
                        ratio = (double)my_ssd->log_group[i]->traff_valid_copy / (double)my_ssd->log_group[i]->traff_erase / (PAGE_NUM);
                }
                else {
                        ratio = 0.00;
                }
                
                printf("GROUP %d[%ld] : %.2f (ERASE: %ld)\n", i, my_ssd->log_group[i]->segment_num, ratio, my_ssd->log_group[i]->traff_erase);
        }
}