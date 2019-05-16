#ifndef MORSE_H
#define MORSE_H

/* This header file just combines send and receive functionality. */

#include "curl/curl.h"
#include "imap_response.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * What type of mail provider should we use?
 */
typedef enum mailprovider_t {
    MailProvider_Google,

    /* not yet supported */
    MailProvider_Outlook
} MailProvider;


typedef struct morse_client_t {
    char *bearertoken;

    char *user_email;

    /* 
     * This is a stateful handle that
     * is authenticated to an IMAP 
     * session.
     */
    CURL *curl_imap;

    /*
     * Who is the mail provider?
     */
    MailProvider mailprovider;
} MorseClient;


/*
 * Creates a mail client with the given provider and
 * logs the user in through the browser.
 * Returns NULL if unsuccessful.
 */
MorseClient* morse_client_login(MailProvider provider);


/* 
 * Logs out and garbage collects a MorseClient.
 */ 
void morse_client_logout(MorseClient *client);

/* 
 * Executes an IMAP command and returns the response.
 * The ImapResponse struct type is defined in "imap_response.h".
 * The response must be freed with imap_response_free(ImapResponse*)
 * Returns NULL if the client's curl handle is missing
 * or the command passed in is NULL.
 */
ImapResponse* morse_client_exec_raw_imap(MorseClient *client, char *command);

#ifdef __cplusplus
}
#endif


#endif