#ifndef IMAP_REQUEST_H
#define IMAP_REQUEST_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct imap_request_t {
    /* the IMAP command to execute */
    char *command;
    
    /* function that executes the command
     * and returns a status code. 
     * the arg is the IMAP command
     */
    int (*send_func)(char*);
} ImapRequest;

#ifdef __cplusplus
}
#endif


#endif

