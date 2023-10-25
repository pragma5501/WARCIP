#include <stdio.h>
#include <stdlib.h>

#define ERR_SUCCESS     0x00
#define ERR_MALLOC      0x01
#define ERR_SSD_INIT    0x02
#define ERR_BLOCK_INIT  0x03
#define ERR_PAGE_INIT   0x04

#define MY_ERROR_TYPE int



void my_assert (MY_ERROR_TYPE const flag) {

        switch (flag) {
        case ERR_SUCCESS:
                break;

        case ERR_MALLOC:
                printf("Malloc is failed\n");
                exit(1);
        case ERR_SSD_INIT:
                printf("SSD Initializer has problem\n");
                exit(1);

        case ERR_BLOCK_INIT:
                printf("BLOCK Initializer has problem\n");
                exit(1);

        case ERR_PAGE_INIT:
                printf("PAGE Initializer has problem\n");
                exit(1);
        }
}

MY_ERROR_TYPE handle_allc_err (void* data) {
        if (data != (void*)NULL) {
                return ERR_SUCCESS;
        }

        return ERR_MALLOC;
}


