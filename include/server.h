#ifndef SERVER_H
#define SERVER_H

#include "strarray.h"
#include "hashtable.h"

#define PORT "1756"  // the port users will be connecting to

#ifdef __cplusplus
extern "C" {
#endif


void* start_server(void *data);

/**
 * Parses a query string into an array of strings
 * as key1, value1, key2, value2, etc.
 * Stores the resulting array length in elem_count.
 * query_str should be pointing at the address of the 
 * '?' char (start of the query string).
 * Caller must return freed buffer.
 */
char** parse_query_str(char *query_str, size_t *elem_count);

Hashtable* create_ht_from_strarray(char **str_array, size_t num_elements);

Hashtable* parse_query_str_to_ht(char *query_str, size_t *num_kv_pairs);

#ifdef __cplusplus
}
#endif

#endif