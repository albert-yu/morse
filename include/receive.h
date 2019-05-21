#ifndef RECEIVE_H
#define RECEIVE_H

#include "curl/curl.h"
#include "endpoints.h"
#include "imap_response.h"
#include "mailbox.h"
#include "statuscodes.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Invokes the authentication process for the user to 
 * log into Gmail
 */
CURL* get_imap_curl_google(void);


/*
 * Retrieves an array of mailboxes
 */
Mailbox* get_mailboxes(CURL *curl);


/*
 * Performs "SELECT {boxname}"
 */
ImapResponse* select_box(CURL *curlhandle, char *box_name);


/*
 * Begin IDLE. Returns 0 if successful.
 */
int begin_idle(CURL *curl);


/*
 * Print out mail
 */
void print_mailboxes(Mailbox *root);


/* 
 * Execute an arbitrary IMAP command.
 */
ImapResponse* morse_exec_imap_stateful(CURL *curl, char *command);

#ifdef __cplusplus
}
#endif

#endif
