#ifndef MEMSTRUCT_H
#define MEMSTRUCT_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Struct to hold the entire JSON
 */
typedef struct memory_struct_t {
    char *memory;
    size_t size;
} MemoryStruct;


/**
 * Initializes a memory struct. 
 * To free, call free(memstruct.memory)
 */
int memory_struct_init(MemoryStruct *chunk);

/**
 * Callback function for handling server response.
 * signature: size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
 */
size_t curl_mem_callback(void *buffer, size_t size, size_t nmemb, void *userp);

#ifdef __cplusplus
}
#endif

#endif
