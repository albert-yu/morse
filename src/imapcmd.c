#include <stdio.h>
#include <stdlib.h>

#include "imapcmd.h"



/*
 * Prepends "SELECT " to a given box name.
 * Is this better with strncat?   
 */
char* imapcmd_select_box(char *box_name) {
    const char *template = "SELECT %s";
    char *buffer = calloc(64, sizeof(*buffer));
    sprintf(buffer, template, box_name);
    return buffer;
}


/*
 * Fetches the text of the message with the given UID
 */
char* imapcmd_uid_fetch_body_text(char *uid) {
    const char *template = "UID FETCH %s BODY[TEXT]";
    char *buffer = calloc(64, sizeof(*buffer));
    sprintf(buffer, template, box_name);
    return buffer;
}



