#include "curlyfries.h"

/*
 * Iterates through a CURL linked list.
 * The second argument is a function that does something
 * with the current index and item.
 */
void iter_list(struct curl_slist *list, 
               void (*str_func)(size_t, char*)) {
    struct curl_slist     *next;
    struct curl_slist     *item;

    item = list;
    size_t cnt = 0;
    do {
        next = item->next;
        str_func(cnt, item->data);
        item = next;
        cnt++;
    } while (next);
}


/*
 * Prints a single item with the given index
 */
void print_item(size_t index, char *data) {
    printf("[%zu]:\t%s\n", index, data);
}


/*
 * Prints out a curl linked list
 */
void print_list(struct curl_slist *list) {
    if(!list) {
        printf("(empty list)\n");
        return;
    }
    iter_list(list, &print_item);
}


/*
 * Gets a CURL handle with XOAuth2 set up
 */
CURL* get_curl_xoauth2(const char *bearertoken,
                       const char *url,
                       const char *username) {
    CURL *curl;
    curl = curl_easy_init();
    if (curl) {
        printf("curl init OK\n");
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // auth        
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        curl_easy_setopt(curl, CURLOPT_LOGIN_OPTIONS, "AUTH=XOAUTH2");
        curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, bearertoken);
        curl_easy_setopt(curl, CURLOPT_SASL_IR, 1L);

        // use SSL
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    }
    return curl;
}


/*
 * Get a CURL handle configured for Google.
 * The URL can be either the SMTP or IMAP one
 */
CURL* get_google_curl(const char *url) {
    CURL *curl;
    char *bearertoken = getgooglebearertoken(); 
    char *username = getgmailaddress(bearertoken);
    curl = get_curl_xoauth2(bearertoken, url, username);

    return curl;
}

