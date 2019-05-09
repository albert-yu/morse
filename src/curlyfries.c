#include "curlyfries.h"


/*
 * Prints out a curl linked list
 */
void print_list(struct curl_slist *list) {
    struct curl_slist     *next;
    struct curl_slist     *item;

    if(!list) {
        printf("(empty list)\n");
        return;
    }

    item = list;
    size_t cnt = 0;
    do {
        next = item->next;
        printf("[%zu]: %s\n", cnt, item->data);
        item = next;
        cnt++;
    } while(next);
}

