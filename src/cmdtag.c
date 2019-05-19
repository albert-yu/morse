#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmdtag.h"
#include "dataconv.h"

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


