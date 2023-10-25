#include "test.h"


int main(int argv, char* argc) {

}

w_driver_t* w_driver_t_init ()
{
        w_driver_t* w_driver = (w_driver_t*)malloc(sizeof(w_driver_t));
        init_centers(w_driver->centers);
        init_t_table(w_driver->t_table);

        
}


double* init_centers (double* centers)
{
        centers = (double*)malloc(sizeof(double) * MAX_CLUSTER_NUM);
        centers[0] = 1;
        for (int i = 1; i < MAX_CLUSTER_NUM; i++) {
                centers[i] = centers[i-1] * 2;
        }

        return centers;
}

double* init_t_table (double* t_table)
{
        t_table = (double*)malloc(sizeof(double) * T_TABLE_NUM);
}


int get_time_table_index (int LBA)
{
        return (int)(LBA / (T_TABLE_PAGE_NUM) );
}

int get_RWI (w_driver_t* w_driver, int LBA, double time) 
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

void cluster_greedy (w_driver_t* w_driver, )
{
        // 1. init_cluster
        // 2. 
        // 
}

void dsam () 
{
        
}

void merge ()
{
        
}

void split () 
{

}