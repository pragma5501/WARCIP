#ifndef __WARCIP_H__
#define __WARCIP_H__

#include <stdio.h>
#include <stdlib.h>
#include "macro.h"
#include "dispatcher.h"
#include "ssd.h"

#define T_TABLE_NUM 3125 * 1250
#define T_TABLE_PAGE_NUM 8

#define MAX_CLUSTER_NUM 16
#define MAX_CLUSTER_PAGE_NUM 512

#define THRESHOLD_DSAM  256
#define THRESHOLD_SPLIT 256 // PAGE_NUM / 2 == 516 / 2;
#define THRESHOLD_MERGE 10
#define STATE_CLOSED -1


typedef struct cluster {
        double center; // average of the sum of RWIs
        int stream_id; // this cluster point open_block[stream_id]
        double num_pages; // the number of pages that this cluster has.
        
        int num_pages_per_term;
} cluster_t;


typedef struct WACRIP_driver {
        cluster_t* clusters;
        double* t_table;

        int cnt_block_filled;

        w_dispatcher_t* sender_dispatcher;
} w_driver_t;

#endif

void sort_clusters (w_driver_t* w_driver);

int WARCIP (w_driver_t* w_driver, double RWI);

w_driver_t* w_driver_t_init (w_driver_t* w_driver);
double* init_clusters (w_driver_t* w_driver);
double* init_t_table (w_driver_t* w_driver);
int get_time_table_index (int LBA);
double get_RWI (w_driver_t* w_driver, int LBA, double time);
void update_time_table (w_driver_t* w_driver,int LBA, double time);


int cluster_greedy (w_driver_t* w_driver, double RWI);
void comp_d_RWI (int* cluster_id, int new_cluster_id,double* d_RWI, double new_d);
void dsam (w_driver_t* w_driver, int cluster_id);


void split (w_driver_t* w_driver, int cluster_id);

void merge (w_driver_t* w_driver, int cluster_id);