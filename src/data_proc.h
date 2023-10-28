#include "WARCIP.h"

#ifndef __DATA_H__
#define __DATA_H__

FILE* open_csv_file (FILE* fp, char* filename);
void close_csv_file (FILE*fp);
void save_header_data (FILE* fp);
void save_cluster_data (FILE* fp, w_driver_t* w_driver);

#endif