#include "httpclient.h"

#include <string.h>
#include <curl/curl.h>

#define MORSE_USERAGENT "morse-cli/1.0"


void http_get(char *url, char *auth, http_callback_func callback, void *writedata) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    // set headers
    struct curl_slist *headers = NULL;

    // // set content type
    // char header_contenttype [64];
    // sprintf(header_contenttype, "Content-Type: %s", content_type);
    // headers = curl_slist_append(headers, header_contenttype);

    // set authorization if applicable
    if (auth) {
        size_t auth_header_buf_size = strlen(auth) + 32;
        char header_auth [auth_header_buf_size];
        sprintf(header_auth, "Authorization: %s", header_auth);
        headers = curl_slist_append(headers, header_auth);
    }

    // get curl handle
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // set callback if specified
        if (callback && writedata) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, writedata);
        }

        // pass our list of custom made headers 
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // set a user agent string
        curl_easy_setopt(curl, CURLOPT_USERAGENT, MORSE_USERAGENT);

        // perform the request, res will get the return code
        res = curl_easy_perform(curl);

        // check for errors 
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
            fprintf(stderr, "URL: %s\n", url);
        }

        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

/**
 * Uses libcurl 
 * https://curl.haxx.se/libcurl/c/http-post.html
 * https://curl.haxx.se/libcurl/c/libcurl-tutorial.html
 */
void http_post(char *url, char *content_type, char *body, char *auth,
        http_callback_func callback, void *writedata) {

    CURL *curl;
    CURLcode res;

    // in Windows, this will init the winsock stuff
    curl_global_init(CURL_GLOBAL_ALL);

    // set headers
    struct curl_slist *headers = NULL;

    // set content type
    char header_contenttype [64];
    sprintf(header_contenttype, "Content-Type: %s", content_type);
    headers = curl_slist_append(headers, header_contenttype);

    // set authorization if applicable
    if (auth) {
        size_t auth_header_buf_size = strlen(auth) + 32;
        char header_auth [auth_header_buf_size];
        sprintf(header_auth, "Authorization: %s", header_auth);
        headers = curl_slist_append(headers, header_auth);
    }

    /* get a curl handle */ 
    curl = curl_easy_init();
    if (curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */ 
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // now specify the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);

        // set callback if specified
        if (callback && writedata) {
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, writedata);
        }
        // defaults to stdout otherwise

        // pass our list of custom made headers 
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // set a user agent string
        curl_easy_setopt(curl, CURLOPT_USERAGENT, MORSE_USERAGENT);

        // perform the request, res will get the return code
        res = curl_easy_perform(curl);

        // check for errors 
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
            fprintf(stderr, "URL: %s\n", url);
            fprintf(stderr, "Content-Type: %s\n", content_type);
            fprintf(stderr, "Body: %s\n", body);
        }

        // always cleanup
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
} 


void http_post_no_auth(char *url, char *content_type, char *body, 
               http_callback_func callback, void *writedata) {

    http_post(url, content_type, body, NULL, callback, writedata);
}


