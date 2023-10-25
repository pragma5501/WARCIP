#include <stdio.h>
#include <stdlib.h>


#define T_TABLE_NUM 3125 * 1250

#define T_TABLE_PAGE_NUM 8

#define MAX_CLUSTER_NUM 16
#define INIT_CLUSTER_NUM 4

typedef struct WACRIP_driver {
        double* centers;
        double* t_table;

} w_driver_t;

typedef struct cluster {
        int stream_id;
} cluster_t;

void dsam ();


void split ();

void merge ();


