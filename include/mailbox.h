#ifndef MAILBOX_H
#define MAILBOX_H

#include <stdlib.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Defines a mailbox folder with
 * parent and child elements
 */
typedef struct mailbox_t {
    char *name;
    Mailbox *parent;

    size_t child_count;
    Mailbox *children;

    /*
     * Attributes returned in
     * the IMAP response such 
     * as HasNoChildren, Important
     */
    size_t attr_count;
    char **attrs;
} Mailbox;

#ifdef __cplusplus
}
#endif


#endif
