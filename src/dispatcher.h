#include <stdio.h>
#include <stdlib.h>

#define MAX_STREAM_NUM 16
#define STATE_CLOSED -1

typedef struct dispatcher_t{
        // index : 
        int* stream_id_q; 
        int head;
        int tail;
        
        int length;

} w_dispatcher_t;


w_dispatcher_t* w_dispatcher_t_init (w_dispatcher_t* my_dispatcher);
void push_stream_id (w_dispatcher_t* my_dispatcher, int stream_id);
int pop_stream_id (w_dispatcher_t* my_dispatcher);
int dispatch(w_dispatcher_t* my_dispatcher, int stream_id);