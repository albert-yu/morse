#include <curl/curl.h>
#include <stdio.h>

#include "authenticate.h"
#include "receive.h"


/**
 * Caller must free returned buffer
 */
char* construct_url(char *base_url, char *path) {
    if (!base_url) {
        fprintf(stderr, "Null base URL supplied.\n");
        return NULL;
    }
    int path_is_nonempty = 1;
    if (!path) {
        path_is_nonempty = 0;
    }

    size_t total_len;
    
    if (path_is_nonempty) {
        total_len = strlen(path) + strlen(base_url);
    }
    else {
        total_len = strlen(base_url);
    }
    // total_len + 2 to account for forward slash
    char *buffer = calloc(total_len + 2, sizeof(*buffer));
    if (!buffer) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    if (path_is_nonempty) {
        sprintf(buffer, "%s/%s", base_url, path);
    }
    else {
        strcpy(buffer, base_url);
    }
    return buffer;
}


int simple_fetch() {
    CURL *curl;
    CURLcode res = CURLE_OK;

    char *username = getgmailaddress();
    if (!username) {
        fprintf(stderr, "Could not get username.\n");
        return -1;
    }

    char *bearertoken = getgooglebearertoken();
    if (!bearertoken) {
        // this should not happen if getgmailaddress() is successful
        fprintf(stderr, "Could not get bearer token.\n");
        return -1;
    }


    // char *mailbox = "INBOX";
    char *mailbox = NULL;
    char *base_url = GOOGLE_IMAPS;

    char *imap_url = construct_url(base_url, mailbox);
    if (!imap_url) {
        fprintf(stderr, "Failed to get IMAP URL.\n");
        return -1;
    }

    curl = curl_easy_init();
    if (curl) {
        printf("curl init OK [IMAP]\n");
        /* Set username and password */
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        // curl_easy_setopt(curl, CURLOPT_PASSWORD, "secret");
        curl_easy_setopt(curl, CURLOPT_URL, imap_url);
        curl_easy_setopt(curl, CURLOPT_LOGIN_OPTIONS, "AUTH=XOAUTH2");
        curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, bearertoken);
        curl_easy_setopt(curl, CURLOPT_SASL_IR, 1L);

        // use SSL
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        // set a timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);

        // verbose output
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "EXAMINE INBOX");

        /* Perform the fetch */
        res = curl_easy_perform(curl);

        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

        /* Always cleanup */
        curl_easy_cleanup(curl);
    }

    free(username);
    free(bearertoken);
    free(imap_url);

    return (int)res;
}