#include "httpclient.h"

#include <string.h>
#include <curl/curl.h>


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
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "morse-cli/1.0");

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


void build_message(char *auth) {
    // CURL *easyhandle;
    // CURLcode res;

    // // in Windows, this will init the winsock stuff
    // curl_global_init(CURL_GLOBAL_ALL);

    // struct curl_slist *headers = NULL;

    // // set authorization if applicable
    // if (auth) {
    //     size_t auth_header_buf_size = strlen(auth) + 32;
    //     char header_auth [auth_header_buf_size];
    //     sprintf(header_auth, "Authorization: %s", header_auth);
    //     headers = curl_slist_append(headers, header_auth);
    // }
    // /* get a curl handle */ 
    // easyhandle = curl_easy_init();
    // if (easyhandle) {
    //     curl_mime *message = curl_mime_init(easyhandle);

    //     /* The inline part is an alternative proposing the html and the text
    //     versions of the e-mail. */
    //     curl_mime *alt = curl_mime_init(easyhandle);

    //     /* HTML message. */
    //     curl_mimepart *part = curl_mime_addpart(alt);
    //     curl_mime_data(part, "<html><body><p>This is HTML</p></body></html>",
    //                       CURL_ZERO_TERMINATED);
    //     curl_mime_type(part, "text/html");

    //     /* Text message. */
    //     part = curl_mime_addpart(alt);
    //     curl_mime_data(part, "This is plain text message",
    //                       CURL_ZERO_TERMINATED);

    //     /* Create the inline part. */
    //     part = curl_mime_addpart(message);
    //     curl_mime_subparts(part, alt);
    //     curl_mime_type(part, "multipart/alternative");
    //     struct curl_slist *headers = curl_slist_append(NULL,
    //                    "Content-Disposition: inline");
    //     curl_mime_headers(part, headers, TRUE);

    //     /* Add the attachment. */
    //     part = curl_mime_addpart(message);
    //     curl_mime_filedata(part, "manual.pdf");
    //     curl_mime_encoder(part, "base64");

    //     /* Build the mail headers. */
    //     headers = curl_slist_append(NULL, "From: me@example.com");
    //     headers = curl_slist_append(headers, "To: you@example.com");

    //     /* Set these into the easy handle. */
    //     curl_easy_setopt(easyhandle, CURLOPT_HTTPHEADER, headers);
    //     curl_easy_setopt(easyhandle, CURLOPT_MIMEPOST, mime);

    //     curl_easy_cleanup(curl);
    // }
    // curl_global_cleanup();   
}
