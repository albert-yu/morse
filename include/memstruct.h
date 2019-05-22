#ifndef MEMSTRUCT_H
#define MEMSTRUCT_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "curl/curl.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Struct to hold the entire JSON
 */
typedef struct memory_struct_t {
    char *memory;
    size_t size;
} MemoryStruct;


/*
 * Initializes a memory struct. 
 * To free, call free(memstruct.memory)
 */
int memstruct_init(MemoryStruct *chunk);


/*
 * Creates a new MemoryStruct on the heap.
 */
MemoryStruct* memstruct_new(void); 


/*
 * Frees a MemoryStruct
 */
void memstruct_free(MemoryStruct* mem_struct);


/*
 * Callback function for handling server response.
 * signature: size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
 */
size_t curl_mem_callback(void *buffer, size_t size, size_t nmemb, void *userp);


/*
 * Callback function for handling verbose response
 */
int curl_mem_debug_callback(CURL *handle,
                            curl_infotype type,
                            char *data,
                            size_t size,
                            void *userptr);


#ifdef __cplusplus
}
#endif

#endif

