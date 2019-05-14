#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "imapcmd.h"

#define DEFAULT_IMAP_CMD_LENGTH 64


/*
 * Prepends "SELECT " to a given box name.
 * Is this better with strncat?   
 */
char* imapcmd_select_box(char *box_name) {
    const char *template = "SELECT \"%s\"";
    char *buffer = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*buffer));
    sprintf(buffer, template, box_name);
    return buffer;
}


/*
 * Creates a folder
 */
char *imapcmd_create_folder(char *foldername) {
    const char *template = "CREATE \"%s\"";
    char *buf = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*buf));
    sprintf(buf, template, foldername);
    return buf;
}


/*
 * Fetches the text of the message with the given ID
 */
char* imapcmd_id_fetch_body_text(size_t id) {
    const char *template = "FETCH %zu BODY[TEXT]";
    char *buffer = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*buffer));
    sprintf(buffer, template, id);
    return buffer;
}


/*
 * Fetches the subject of the message WITHOUT opening it (marking
 * as read)
 */
char* imapcmd_id_get_subject(size_t id) {
    const char *template = "FETCH %zu BODY.PEEK[HEADER.FIELDS (SUBJECT)]";
    char *retval = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*retval));
    sprintf(retval, template, id);
    return retval;
}


char* imapcmd_list_boxes() {
    const char *list_cmd = "LIST \"\" *";
    char *retval = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*retval));
    strcpy(retval, list_cmd);
    return retval;
}


/*
 * Command for fetching emails
 */
char* imapcmd_fetch_messages(size_t start_id, size_t length) {
    const char *list_messages_cmd_template = "FETCH %zu:%zu (FLAGS)";
    char *retval = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*retval));
    sprintf(retval, list_messages_cmd_template, 
        start_id, start_id + length - 1);
    return retval;
}


char *imapcmd_list_messages(size_t total_msg_count, size_t length) {
    if (length > total_msg_count) {
        fprintf(stderr, "Cannot have length (%zu) greater than total_msg_count (%zu).\n",
            length, total_msg_count);
    }

    const char *list_messages_cmd_template = "FETCH %zu:%zu (FLAGS)";
    char *retval = calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*retval));
    sprintf(retval, list_messages_cmd_template, 
        total_msg_count - length + 1, total_msg_count);
    return retval;
}
