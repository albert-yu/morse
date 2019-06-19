/*
 * This file contains methods that help with CURL data structures.
 */ 


#ifndef CURLYFRIES_H
#define CURLYFRIES_H

#include "curl/curl.h"
#include "authenticate.h"


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


/*
 * Gets a CURL handle with XOAuth2 set up
 */
CURL* get_curl_xoauth2(const char *bearertoken,
                       const char *url,
                       const char *username);


/*
 * Get a CURL handle configured for Google.
 * The URL can be either the SMTP or IMAP one
 */
CURL* get_google_curl(const char *url); 


#ifdef __cplusplus
}
#endif

#endif

