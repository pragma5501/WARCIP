#include "WARCIP.h"


int compare_centers(const void* a, const void* b) {
        if (((cluster_t*)a)->center > ((cluster_t*)b)->center) {
                return 1;
        }
        return -1;
}

void sort_clusters (w_driver_t* w_driver) 
{
        int clusters_size = sizeof(w_driver->clusters) / sizeof(cluster_t);
        qsort(w_driver->clusters, clusters_size, sizeof(cluster_t), compare_centers);

}

void make_unavailable_cluster (w_driver_t* w_driver, int cluster_id) 
{
        w_driver->clusters[cluster_id].num_pages = 0;
        
        push_stream_id (w_driver->sender_dispatcher, w_driver->clusters[cluster_id].stream_id);
        w_driver->clusters[cluster_id].stream_id = (STATE_CLOSED);
}

int WARCIP(w_driver_t*w_driver, double RWI) 
{
        int cluster_id = cluster_greedy(w_driver, RWI);

        return cluster_id;
}

w_driver_t* w_driver_t_init (w_driver_t* w_driver)
{
        w_driver = (w_driver_t*)malloc(sizeof(w_driver_t));
        init_clusters(w_driver);
        init_t_table(w_driver);

        w_driver->cnt_block_filled = 0;
        return w_driver;
}


double* init_clusters (w_driver_t* w_driver)
{
        w_driver->clusters = (cluster_t*)malloc(sizeof(cluster_t) * MAX_CLUSTER_NUM);
        w_driver->clusters[0].center = 1;
 
 
        for (int i = 1; i < MAX_CLUSTER_NUM; i++) {
                w_driver->clusters[i].center = w_driver->clusters[i-1].center * 4;
        }

        for (int i = 0; i < MAX_CLUSTER_NUM; i++) {
                w_driver->clusters[i].stream_id = (STATE_CLOSED);
                w_driver->clusters[i].num_pages = 0;
        }
}

void* reinit_cluster (w_driver_t* w_driver, int cluster_id)
{
        w_driver->clusters[cluster_id].num_pages = 0;

        // find near two center
        // center is always sorted.
        double new_center = 0;
        int near_c_1 = -1;
        int near_c_2 = -1;

        if (cluster_id == 0) {
                near_c_1 = 0;
                near_c_2 = 1;
        }
        else if (cluster_id == (MAX_CLUSTER_NUM) - 1) {
                near_c_1 = (MAX_CLUSTER_NUM) - 1;
                near_c_2 = (MAX_CLUSTER_NUM) - 2;
        }
        else {
                near_c_1 = cluster_id - 1;
                near_c_2 = cluster_id + 1;
        }

        new_center += w_driver->clusters[near_c_1].center;
        new_center += w_driver->clusters[near_c_2].center;
        new_center /= 2;

        w_driver->clusters[cluster_id].center = new_center;
        sort_clusters(w_driver);

}

double* init_t_table (w_driver_t* w_driver)
{
        w_driver->t_table = (double*)malloc(sizeof(double) * T_TABLE_NUM);
        for (int i = 0; i < T_TABLE_NUM; i++) {
                w_driver->t_table[i] = 0;
        }
}

int get_time_table_index (int LBA)
{
        return (int)(LBA / (T_TABLE_PAGE_NUM) );
}

double get_RWI (w_driver_t* w_driver, int LBA, double time) 
{
        int index = get_time_table_index(LBA);
        
        double reported_time = w_driver->t_table[index];
        double RWI = time - reported_time;

        return RWI;
}

void update_time_table (w_driver_t* w_driver,int LBA, double time)
{
        int index = get_time_table_index(LBA);
        w_driver->t_table[index] = time;
}

int cluster_greedy (w_driver_t* w_driver, double RWI)
{
        double d_RWI = ABS(w_driver->clusters[0].center - RWI);
        int cluster_id = 0;

        // select proper cluster
        
        for(int i = 1; i < (MAX_CLUSTER_NUM); i++) {
                double tmp = ABS(w_driver->clusters[i].center - RWI);
                comp_d_RWI(&cluster_id, i, &d_RWI, tmp);
        }

        // update cluster
        //printf("selected cluster : %d\n", cluster_id);
        cluster_t* cluster = w_driver->clusters + cluster_id;
        cluster->center *= (cluster->num_pages++);
        cluster->center += RWI;
        cluster->center /= cluster->num_pages;

        //printf("cluster %d's new center : %f\n", cluster_id, cluster->center);
        // check if cluster closed
        if (cluster->num_pages >= (MAX_CLUSTER_PAGE_NUM)) {
                reinit_cluster(w_driver, cluster_id);
        }

        // dsam trigger
        if ((w_driver->cnt_block_filled + 125) % (THRESHOLD_DSAM) == 0) {
                dsam(w_driver, cluster_id);
        }
        
        return cluster_id;
}



void comp_d_RWI (int* cluster_id, int new_cluster_id,double* d_RWI, double new_d)
{
        if (new_d < *d_RWI) {
                *d_RWI = new_d;
                *cluster_id = new_cluster_id;
        }
}

void dsam (w_driver_t* w_driver, int cluster_id) 
{
        if (w_driver->clusters[cluster_id].num_pages >= (THRESHOLD_SPLIT)) {
                split(w_driver, cluster_id);
                return;
        }
        
        for (int i = 0; i < MAX_CLUSTER_NUM; i++) {
                if (w_driver->clusters[i].stream_id == (STATE_CLOSED)) {
                        break;
                }
                if (w_driver->clusters[i].num_pages <= (THRESHOLD_MERGE)) {
                     merge(w_driver, i);
                     w_driver->clusters[i].num_pages = 0;
                     break;
                }
        }
        
}


void split_cluster_delta (w_driver_t* w_driver, int cluster_id, int victim_id) 
{
        double delta = 0.01;
        w_driver->clusters[victim_id].center   = w_driver->clusters[cluster_id].center + delta;
        w_driver->clusters[cluster_id].center -= delta;


}

int find_closest_center (w_driver_t* w_driver, int cluster_id) 
{
        int id1 = 0;
        int id2 = 0;

        // Case 1 : minimum RWI average
        if (cluster_id == 0) {
                id2 = 1;
        }
        // Case 2 : maximum RWI average
        else if (cluster_id == (MAX_CLUSTER_NUM) - 1) {
                id2 = cluster_id - 1;// ;
        }
        else {
                id1 = cluster_id - 1;
                id2 = cluster_id + 1;
        }
        double tmp1 = w_driver->clusters[id1].center;
        double tmp2 = w_driver->clusters[id2].center;

        // tmp1 < cur_center < tmp2
        double cur_center = w_driver->clusters[cluster_id].center;
        if ( (ABS(tmp1 - cur_center)) < (ABS(tmp2 - cur_center)) ) {
                return id1;
        } else {
                return id2;
        }
}

void split_cluster (w_driver_t* w_driver, int cluster_id, int victim_id)
{
        int closest_id = find_closest_center(w_driver, cluster_id);

        double cur_center = w_driver->clusters[cluster_id].center;
        double closest_center = w_driver->clusters[closest_id].center;

        w_driver->clusters[victim_id].center = (cur_center + closest_center) / 2;
}


void split (w_driver_t* w_driver, int cluster_id) 
{
        cluster_t vtm = w_driver->clusters[cluster_id];
        if (cluster_id == (MAX_CLUSTER_NUM) - 1) {
                return;
        }

        for (int i = 0; i < MAX_CLUSTER_NUM; i++) {
                // set new cluster that absorb burst I/O
                if (w_driver->clusters[i].num_pages == 0) {
                        split_cluster(w_driver, cluster_id, i);
                        
                        break;
                }
        }

        // sort clusters
        sort_clusters(w_driver);
}

void merge_cluster (w_driver_t* w_driver, int cluster_id)
{
        int closest_id = find_closest_center(w_driver, cluster_id);
        double cur_center = w_driver->clusters[cluster_id].center;
        double closest_center = w_driver->clusters[closest_id].center;

        w_driver->clusters[cluster_id].center = (cur_center + closest_center) / 2;

        // push free cluster
        
        make_unavailable_cluster(w_driver, cluster_id);
}

void merge (w_driver_t* w_driver, int cluster_id)
{
        if (w_driver->sender_dispatcher->length <= 1) return;
        
        merge_cluster(w_driver, cluster_id);
        sort_clusters(w_driver);
}



