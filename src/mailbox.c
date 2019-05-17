#include <string.h>
#include "mailbox.h"


Mailbox* mailbox_create_new(char *name) {
    Mailbox *newbox = NULL;
    newbox = malloc(sizeof(*newbox));

    if (newbox) {
        newbox->name = strdup(name);
        newbox->parent = NULL;
        newbox->children = NULL;
        newbox->child_count = 0;
        newbox->attr_count = 0;
        newbox->attrs = NULL;
    }
    return newbox;
}


Mailbox* mailbox_create_new_root() {
    Mailbox *newbox = mailbox_create_new(ROOT_MAILBOX_NAME); 
    return newbox;
}



