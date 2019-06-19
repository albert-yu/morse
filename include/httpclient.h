#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Function prototype for handling an HTTP response
 */
typedef size_t http_callback_func(void*, size_t, size_t, void*);


void http_get(char *url, char *auth, http_callback_func callback, void *writedata);


/**
 * Sends an HTTP POST.
 * The callback function must be of prototype 
 * size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);
 * You can pass in NULL for the callback to output to stdout.

 * @auth - something like "Bearer xxxxxx"
 */
void http_post(char *url, char *content_type, char *body, char *auth, http_callback_func callback, void *writedata);
void http_post_no_auth(char *url, char *content_type, char *body, http_callback_func callback, void *writedata); 

#ifdef __cplusplus
}
#endif

#endif 

