#include "WARCIP.h"
#include "ssd.h"

#ifndef __DATA_H__
#define __DATA_H__

FILE* open_csv_file (FILE* fp, char* filename);
void close_csv_file (FILE*fp);
void save_header_data (FILE* fp);
void save_cluster_data (FILE* fp, ssd_t* my_ssd,w_driver_t* w_driver);

#endif