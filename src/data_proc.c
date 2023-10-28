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
                fprintf(fp, "Cluster_%d", i);
                if (i == (MAX_CLUSTER_NUM) - 1) break;
                fprintf(fp, ",");
        }
        fprintf(fp, "\n");
}

void save_cluster_data (FILE* fp, w_driver_t* w_driver)
{
        // save cluster data
        for (int i = 0; i < (MAX_CLUSTER_NUM); i++) {
                fprintf(fp, "%.5f", w_driver->clusters[i].center);
                if (i == (MAX_CLUSTER_NUM) - 1) break;
                fprintf(fp, ",");
        }
        fprintf(fp, "\n");
}
