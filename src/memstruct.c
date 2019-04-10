#include <string.h>
#include "memstruct.h"


void memory_struct_init(MemoryStruct *chunk) {
    chunk->memory = malloc(1);  /* will be grown as needed by the realloc*/ 
    chunk->size = 0;    /* no data at this point */ 
}


/**
 * Callback function for handling server response.
 * signature: size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
 */
size_t curl_mem_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;

    MemoryStruct *mem = (MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        /* out of memory! */ 
        fprintf(stderr, "Not enough memory (realloc returned NULL).\n");
        exit(1);
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), buffer, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}