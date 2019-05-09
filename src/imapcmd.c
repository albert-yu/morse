#include <stdio.h>
#include <stdlib.h>

#include "imapcmd.h"

#define DEFAULT_IMAP_CMD_LENGTH 64

/*
 * Prepends "SELECT " to a given box name.
 * Is this better with strncat?   
 */
char* imapcmd_select_box(char *box_name) {
    const char *template = "SELECT %s";
    char *buffer = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*buffer));
    sprintf(buffer, template, box_name);
    return buffer;
}


/*
 * Fetches the text of the message with the given UID
 */
char* imapcmd_uid_fetch_body_text(char *uid) {
    const char *template = "UID FETCH %s BODY[TEXT]";
    char *buffer = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*buffer));
    sprintf(buffer, template, uid);
    return buffer;
}


/*
 * Fetches the subject of the message WITHOUT opening it (marking
 * as read)
 */
char* imapcmd_uid_get_subject(char *uid) {
    const char *template = "UID FETCH %s BODY.PEEK[HEADER.FIELDS (SUBJECT)]";
    char *retval = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*retval));
    sprintf(retval, template, uid);
    return retval;
}


char* imapcmd_list_boxes() {
    const char *list_cmd = "LIST \"\" *";
    char *retval = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*retval));
    strcpy(retval, list_cmd);
    return retval;
}


