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

    // increase the buffer size if needed
    if (mbox->attr_bufsize == mbox->attr_count) {
        char **new_attrs;
        new_attrs = realloc(mbox->attrs, mbox->attr_bufsize * 2 * sizeof(*new_attrs));
        if (!new_attrs) {
            fprintf(stderr, "Not enough mem for new attributes.\n");
            return;
        }
        // successful realloc means old pointer is invalid
        mbox->attrs = new_attrs;
        mbox->attr_bufsize *= 2;
    }

    // add the attribute (duplicate the string)
    mbox->attrs[mbox->attr_count] = strdup(attribute);
    mbox->attr_count += 1;
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


void mailbox_free_tree(Mailbox *rootbox) {
    if (!rootbox) {
        return;
    }

    // free children recursively
    if (rootbox->child_count > 0) {
        for (size_t i = 0; i < rootbox->child_count; i++) {
            mailbox_free_tree((rootbox->children)[i]);
        }
        free(rootbox->children);
    }
    
    // free attributes
    if (rootbox->attr_count > 0) {
        // all attributes are copied strings, so
        // they need to be freed
        char **attrs = rootbox->attrs;
        for (size_t i = 0; i < rootbox->attr_count; i++) {
            free(attrs[i]);
        }
        free(attrs);
    }

    // names are copied, so need to be freed
    if (rootbox->name) {
        free(rootbox->name);
    }

    free(rootbox);
    rootbox = NULL;
}
