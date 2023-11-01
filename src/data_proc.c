#include "data_proc.h"



FILE* open_csv_file (FILE* fp, char* filename)
{
        fp = fopen(filename, "w");
        if (fp == NULL) {
                perror("Opening file is failed");
        }
        return fp;
}

void close_csv_file (FILE*fp) 
{
        fclose(fp);
}

void save_header_data (FILE* fp)
{
        for (int i = 0; i < (MAX_CLUSTER_NUM); i++) {
                fprintf(fp, "Cluster_%d,", i);
                
        }
        fprintf(fp, "WAF");
        fprintf(fp, "\n");
}

void save_cluster_data (FILE* fp, ssd_t* my_ssd, w_driver_t* w_driver)
{
        // save cluster data
        for (int i = 0; i < (MAX_CLUSTER_NUM); i++) {
                if(w_driver->clusters[i].stream_id == (STATE_CLOSED) ) {
                        fprintf(fp, "NaN,");
                        continue;
                }
                fprintf(fp, "%.5f,", w_driver->clusters[i].center);
                
        }
        fprintf(fp, "%.2f", get_WAF(my_ssd));
        fprintf(fp, "\n");
}
