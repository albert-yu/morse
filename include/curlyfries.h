#ifndef CURLYFRIES_H
#define CURLYFRIES_H

#include <curl/curl.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Iterates through a CURL linked list.
 * The second argument is a function that does something
 * with the current index and item.
 */
void iter_list(struct curl_slist *list, 
               void (*str_func)(size_t, char*));


/*
 * Prints out the items in a CURL linked list to stdout.
 */
void print_list(struct curl_slist *list);

#ifdef __cplusplus
}
#endif

#endif
