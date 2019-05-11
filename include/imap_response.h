/*
 * Data Transfer Object to hold the IMAP response
 */

#ifndef IMAP_RESPONSE_H
#define IMAP_RESPONSE_H

#include "memstruct.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct imap_response_t {
    /* The response data. Should be NULL if failed */
    MemoryStruct *data;

    /* Did any errors occur? A success is 0. */
    int status;
} ImapResponse;


#ifdef __cplusplus
}
#endif

#endif
