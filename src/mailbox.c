#include <string.h>
#include <stdio.h>
#include "mailbox.h"


Mailbox* mailbox_create_new(char *name) {
    Mailbox *newbox = NULL;
    newbox = malloc(sizeof(*newbox));

    if (newbox) {
        newbox->name = strdup(name);
        newbox->parent = NULL;
        newbox->children = NULL;
        newbox->child_count = 0;
        newbox->child_bufsize = 0;
        newbox->attr_count = 0;
        newbox->attrs = NULL;
        newbox->attr_bufsize = 0;
    }
    return newbox;
}


Mailbox* mailbox_create_new_root() {
    Mailbox *newbox = mailbox_create_new(ROOT_MAILBOX_NAME); 
    return newbox;
}

void mailbox_add_attr(Mailbox *mbox, char *attribute) {
    if (!mbox) {
        fprintf(stderr, "Mailbox cannot be NULL when adding attr.\n");
        return;
    }

    if (!mbox->attrs) {
        mbox->attr_bufsize = 1;
        mbox->attrs = malloc(mbox->attr_bufsize * sizeof(*(mbox->attrs)));
    }

/*
 * I suppose that the way to check if this operation succeeds
 * is to check if the parent's child_count increases by 1 after.
 * And/or if the child's parent is equivalent to the parent passed in.
 */
void mailbox_add_child(Mailbox *parent, Mailbox *child) {
    char *strformat = "Mailbox %s cannot be NULL when adding"
                      "a child.\n";
    if (!parent) {
        fprintf(stderr, strformat, "parent");
        return;
    }

    if (!child) {
        fprintf(stderr, strformat, "child");
        return;
    }
   
    if (child->parent) {
        fprintf(stderr, "Cannot add child with existing parent.\n");
        return;
    }

    // save some typing
    Mailbox *p = parent;

    // check for NULL children
    if (!p->children) {
        p->child_bufsize = 1;
        
        // sizeof(p) should be the size of the pointer, not
        // the Mailbox struct
        p->children = malloc(p->child_bufsize * sizeof(p));    
    }

    // increase buffer size if needed
    if (p->child_count == p->child_bufsize) {
        Mailbox **new_children;
        new_children = realloc(
            p->children, p->child_bufsize * 2 * sizeof(*new_children));
        if (new_children) {
            // realloc successful
            p->child_bufsize *= 2;
            p->children = new_children;

            // old pointer is invalidated.
        }
        else {
            fprintf(stderr, "Could not reallocate memory.\n");
            // leave old pointer as-is
            return;
        }
    }

    // add the child to the parent
    Mailbox *c = child;
    p->children[p->child_count] = c;
    p->child_count += 1;

    // add the parent to the child
    c->parent = p;
}


