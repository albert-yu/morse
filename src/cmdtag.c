#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmdtag.h"
#include "dataconv.h"
#include "imapcmd.h"

#define IMAP_CMD_TAG_PREFIX "MORSE"

/*
 * Pretty arbitrary
 */
#define MAX_TAG_BUF_SIZE 32 

/*
 * Let's send a max of 10 ^ MAX_DIGIT_LENGTH 
 * commands per session 
 */
#define MAX_DIGIT_LENGTH 5 


/*
 * Our dear command counter.
 */
static size_t tag_id = 0;


/*
 * Gets the next tag to be prepended to an IMAP command.
 * Returns NULL if failed.
 * Otherwise, caller should free returned buffer.
 */
char* getnexttag() {
    char *result_tag = NULL;

    // increment the tag_id
    __atomic_fetch_add(&tag_id, 1, __ATOMIC_SEQ_CST);     
    size_t num = tag_id; 

    // create the number formatted as a string
    result_tag = 
        calloc(MAX_TAG_BUF_SIZE, sizeof(*result_tag));
    char *padded_number = 
        size_t_to_str_padded(num, MAX_DIGIT_LENGTH);   
    if (padded_number && result_tag) {
        strcpy(result_tag, IMAP_CMD_TAG_PREFIX);
        strcat(result_tag, padded_number);
    }
    if (padded_number) {
        free(padded_number);
    }
    
    return result_tag;
}


/*
 * Prepends the command tag to a given IMAP 
 * command. E.g.
 * SELECT INBOX -> A001 SELECT INBOX
 * Returns NULL if failed.
 */
char* add_tag_to_cmd(const char *imap_cmd) {
    char *final_cmd = NULL;
    char *tag = getnexttag(); 
    if (tag) {
        // DEFAULT_IMAP_CMD_LENGTH is defined in "imapcmd.h"
        final_cmd = 
            calloc(DEFAULT_IMAP_CMD_LENGTH, sizeof(*final_cmd));
        if (final_cmd) {
            // concatenate the prefix plus a space 
            sprintf(final_cmd, "%s %s", tag, imap_cmd);
        }
        free(tag);
    }
    return final_cmd;
}


/*
 * Appends \r\n to the given command
 * Return NULL if mem alloc fails or
 * imap_cmd is too long
 */
char* add_carriage_ret(const char *imap_cmd) {
    char *retval = NULL;
    if (!imap_cmd) {
        return NULL;
    }
    // need room for \r\n\0
    size_t max_input_len = 
        DEFAULT_IMAP_CMD_LENGTH - 3;
    if (strlen(imap_cmd) > max_input_len) {
        return NULL;
    }
    retval = calloc(DEFAULT_IMAP_CMD_LENGTH,
                sizeof(*retval));
    if (retval) {
        strcpy(retval, imap_cmd);
        strcat(retval, "\r\n");
    }
    return retval;
}


