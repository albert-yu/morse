#ifndef CURLYFRIES_H
#define CURLYFRIES_H

#include <curl/curl.h>


#ifdef __cplusplus
extern "C" {
#endif

void print_list(struct curl_slist *list);

#ifdef __cplusplus
}
#endif

#endif
