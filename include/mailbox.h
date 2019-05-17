#ifndef MAILBOX_H
#define MAILBOX_H

#include <stdlib.h>


/* 
 * This is the name attribute of the root mailbox.
 */
#define ROOT_MAILBOX_NAME "iamroot"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Defines a mailbox folder with
 * parent and child elements
 */
struct mailbox_t {
    char *name;

    /*
     * A given mailbox can only have one parent.
     * If it's the root, then parent is NULL.
     */
    struct mailbox_t *parent;

    size_t child_count;
    struct mailbox_t *children;

    /*
     * Attributes returned in
     * the IMAP response such 
     * as HasNoChildren, Important
     */
    size_t attr_count;
    char **attrs;
};


typedef struct mailbox_t Mailbox;


/*
 * Allocates a new mailbox on the heap.
 */
Mailbox* mailbox_create_new(char *name);


/*
 * Allocates a new root mailbox on the heap.
 */
Mailbox* mailbox_create_new_root(void);

#ifdef __cplusplus
}
#endif


#endif
