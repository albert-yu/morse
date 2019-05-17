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

    /*
     * Array of pointers, I think, is better than
     * copying the values of the children into a
     * memory array.
     */
    size_t child_count;
    size_t child_bufsize; // memory alloc'd for how many children?
    struct mailbox_t **children;

    /*
     * Attributes returned in
     * the IMAP response such 
     * as HasNoChildren, Important
     */
    size_t attr_count;
    size_t attr_bufsize; // mem alloc'd for how many attrs?
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


/*
 * Adds a heap-allocated child to the parent node.
 * Caller should verify that the parent's child count
 * has increased by 1
 */
void mailbox_add_child(Mailbox *parent, Mailbox *child);


/*
 * Frees an entire tree with the given parent
 * node.
 */
void mailbox_free_tree(Mailbox *rootbox);


/* 
 * Add an attribute to an existing Mailbox
 */
void mailbox_add_attr(Mailbox *mbox, char *attribute);

#ifdef __cplusplus
}
#endif


#endif
