#include "data_proc.h"

w_dispatcher_t* w_dispatcher_t_init (w_dispatcher_t* my_dispatcher)
{
        my_dispatcher = (w_dispatcher_t*)malloc(sizeof(w_dispatcher_t));
        
        my_dispatcher->stream_id_q = (int*)malloc(sizeof(int) * (MAX_STREAM_NUM));
        my_dispatcher->head = 0;
        my_dispatcher->tail = -1;
        my_dispatcher->length = 0;
        for (int i = 0; i < (MAX_STREAM_NUM); i++) {
                push_stream_id(my_dispatcher, i);
        }

}

void push_stream_id (w_dispatcher_t* my_dispatcher, int stream_id)
{

        my_dispatcher->tail = (my_dispatcher->tail + 1) % (MAX_STREAM_NUM);
        my_dispatcher->stream_id_q[my_dispatcher->tail] = stream_id;
        
        my_dispatcher->length++;
}
int pop_stream_id (w_dispatcher_t* my_dispatcher)
{
        if (my_dispatcher->length <= 0) {
                printf("Error: Stream Queue is Empty");
                return (STATE_CLOSED);
        }

        int stream_id = my_dispatcher->stream_id_q[my_dispatcher->head];

        my_dispatcher->head = (my_dispatcher->head + 1) % (MAX_STREAM_NUM);
        my_dispatcher->length--;
        return stream_id;
} 

int dispatch(w_dispatcher_t* my_dispatcher, int stream_id) {
        if (stream_id == (STATE_CLOSED)) {
                
                return pop_stream_id(my_dispatcher);
        }

        return stream_id;
}