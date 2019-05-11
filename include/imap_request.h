#ifndef IMAP_REQUEST_H
#define IMAP_REQUEST_H

#include "imap_response.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct imap_request_t {
    /* the IMAP command to execute */
    char *command;
    
    /* function that executes the command
     * and returns an ImapResponse. 
     * the arg is the IMAP command
     */
    ImapResponse* (*send_func)(char*);
} ImapRequest;


#ifdef __cplusplus
}
#endif


#endif

