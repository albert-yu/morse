#include <string.h>
#include "memstruct.h"


int memstruct_init(MemoryStruct *chunk) {
    chunk->memory = malloc(1);  /* will be grown as needed by the realloc*/ 
    if (chunk->memory == NULL) {
        fprintf(stderr, "Cannot allocate memory for MemoryStruct.\n");
        return ENOMEM;
    }
    chunk->size = 0;    /* no data at this point */ 
    return 0;
}


MemoryStruct* memstruct_new() {
    MemoryStruct *new_mem = malloc(sizeof(*new_mem));
    if (new_mem == NULL) {
        fprintf(stderr, "Cannot allocate memory for new MemoryStruct.\n");
        return NULL;
    }
    new_mem->memory = malloc(1);
    if ((new_mem->memory) == NULL) {
        fprintf(stderr, "Cannot allocate MemoryStruct->memory.\n");
        free(new_mem);
        return NULL;
    }
    new_mem->size = 0;
    return new_mem;
}


/*
 * Frees the char array, then frees the struct.
 */
void memstruct_free(MemoryStruct *mem_struct) {
    if (mem_struct) {
        if (mem_struct->memory) {
            free(mem_struct->memory);
        }
        free(mem_struct);
    }
    mem_struct = NULL;
}


/*
 * Callback function for handling server response.
 * signature: size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
 */
size_t curl_mem_callback(void *buffer, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;

    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        /* out of memory! */ 
        fprintf(stderr, "Not enough memory (realloc returned NULL).\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), buffer, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}


/*
 * Print out the data to stdout
 */
void debug_print(char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        putchar(data[i]);
        if (i == size - 1) {
            printf("foo");
        }
    }
}


/*
 * Callback function that puts verbose data into the v_data member
 * This can be used alongside curl_mem_callback.
 */
int curl_mem_debug_callback(CURL *handle,
                            curl_infotype type,
                            char *data,
                            size_t size,
                            void *userptr) {
    int retcode = 0;
    MemoryStruct *mem = (MemoryStruct *)userptr;
    // + 1 for the zero terminated
    char *ptr = realloc(mem->memory, mem->size + size + 1);
    if (!ptr) {
        /* some way to handle the error */
        fprintf(stderr, "Not enough memory (realloc returned NULL).\n");
        return 0;
    }

    // debug_print(data, size);

    // // switch block not used thus far
    // char *text;
    // switch (type) {
    //     case CURLINFO_TEXT:
    //         fprintf(stderr, "== Info: %s", data);
    //         /* FALLTHROUGH */
    //     default: /* in case a new one is introduced to shock us */
    //         return 0;

    //     case CURLINFO_HEADER_OUT:
    //         text = "=> Send header";
    //         break;
    //     case CURLINFO_DATA_OUT:
    //         text = "=> Send data";
    //         break;
    //     case CURLINFO_SSL_DATA_OUT:
    //         text = "=> Send SSL data";
    //         break;
    //     case CURLINFO_HEADER_IN:
    //         text = "<= Recv header";
    //         break;
    //     case CURLINFO_DATA_IN:
    //         text = "<= Recv data";
    //         debug_print(data, size);
    //         break;
    //     case CURLINFO_SSL_DATA_IN:
    //         text = "<= Recv SSL data";
    //         break;
    // } 
    
    /* copy to memstruct */
    // old mem->memory is invalid if realloc successful
    mem->memory = ptr;     

    // append the data to existing mem struct
    memcpy(&(mem->memory[mem->size]), data, size);

    // adjust size
    mem->size += size;

    // add null-term
    mem->memory[mem->size] = '\0';

    return retcode;
}


