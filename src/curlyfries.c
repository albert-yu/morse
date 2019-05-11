#include "curlyfries.h"


// /*
//  * Prints out a curl linked list
//  */
// void print_list(struct curl_slist *list) {
//     struct curl_slist     *next;
//     struct curl_slist     *item;

//     if(!list) {
//         printf("(empty list)\n");
//         return;
//     }

//     item = list;
//     size_t cnt = 0;
//     do {
//         next = item->next;
//         printf("[%zu]: %s\n", cnt, item->data);
//         item = next;
//         cnt++;
//     } while(next);
// }


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
    } while(next);
}


/*
 * Prints a single item with the given index
 */
void print_item(size_t index, char *data) {
    printf("[%zu]: %s\n", index, data);
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
