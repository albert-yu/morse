#ifndef STRARRAY_H
#define STRARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

// Methods for working with string arrays

/**
 * Print a string array's contents.
 * arr_size - could be the number of elements
 */
void prn_strarray(char **str_array, size_t arr_size);


/**
 * Copies a string to an array of strings at the specified index.
 * WARNING: PLEASE DO NOT ADD A STRING TO AN INDEX W/ EXISTING STRING, 
 * FOR IT WILL CAUSE A MEMORY LEAK
 */
void strarray_setstring(char **str_array, const char *str_toadd, size_t index);


/**
 * Frees an array of strings
 */
void strarray_free(char **str_array, size_t arr_size);


/**
 * Copies a string array from source to destination
 */
void strarray_cpy(char **destination, char **source, size_t num_elements);


/**
 * Re-sizes a string array to double its original size, 
 * returning the new buffer.
 * str_array - the array we will reallocate
 * new_size - the newly allocated buffer
 * old_size - the size of the old buffer
 * num_elements - the number of elements in the array
 */
// size_t strarray_realloc(char **str_array, size_t old_size, size_t num_elements);
char** strarray_realloc(char **str_array, size_t new_size, size_t old_size, size_t num_elements);

#ifdef __cplusplus
}
#endif

#endif