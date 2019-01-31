#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "strarray.h"



void strarray_setstring(char **str_array, const char *str_toadd, size_t index) {
    size_t buf_size = strlen(str_toadd) + 1;
    str_array[index] = (char*)calloc(buf_size, sizeof(char));
    strcpy(str_array[index], str_toadd);
}

void strarray_setstringfree(char **str_array, const char *str_toadd, size_t index) {
    size_t buf_size = strlen(str_toadd) + 1;
    if (str_array[index]) {
        free(str_array[index]);
    }
    str_array[index] = (char*)calloc(buf_size, sizeof(char));
    strcpy(str_array[index], str_toadd);
}


void prn_strarray(char **str_array, size_t arr_size) {
    for (size_t i = 0; i < arr_size; i++) {
        char *curr_str = str_array[i];
        printf("%zu: %s\n", i, curr_str);
    }
}


void strarray_free(char **str_array, size_t arr_size) {
    for (size_t i = 0; i < arr_size; i++) {
        if (str_array[i])
            free(str_array[i]);
    }
    free(str_array);
}


void strarray_cpy(char **destination, char **source, size_t num_elements) {
    for (size_t i = 0; i < num_elements; i++) {
        if (source[i]) {
            char *curr_str = source[i];
            // printf("%zu: %s\n", i, curr_str);
            strarray_setstring(destination, curr_str, i);
        }        
    }
}


char** strarray_realloc(char **str_array, size_t new_size, size_t old_size, size_t num_elements) {
    if (new_size < num_elements) {
        // maybe change the logic here to cause an error?
        new_size = num_elements;
    }

    char **new_array = calloc(new_size, sizeof(*new_array));
    char **old_array = str_array;  // point to old array
    strarray_cpy(new_array, old_array, num_elements);

    // gc old array
    strarray_free(old_array, old_size);

    return new_array;
}

