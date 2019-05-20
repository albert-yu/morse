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
    free(mem_struct->memory);
    free(mem_struct);
    mem_struct = NULL;
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
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), buffer, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}
