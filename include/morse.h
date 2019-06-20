#ifndef MORSE_H
#define MORSE_H

/* This module defines the public API */

#include "curl/curl.h"
#include "imap_response.h"
#include "mailbox.h"
#include "receive.h"
#include "send.h"

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
 * Sends an email. Does not affect or depend 
 * on the state of the curl_imap handle.
 * SmtpRequest is defined in send.h.
 * Returns a status code of 0 if successful.
 */
int morse_client_sendmail(MorseClient *client, SmtpRequest *smtp_request);


/* 
 * Executes an IMAP command and returns the response.
 * The ImapResponse struct type is defined in "imap_response.h".
 * The response must be freed with imap_response_free(ImapResponse*)
 * Returns NULL if the client's curl handle is missing
 * or the command passed in is NULL.
 */
ImapResponse* morse_client_exec_raw_imap(MorseClient *client, 
                                         const char *command, 
                                         int verbose);


/*
 * Returns a list of mailboxes. Returns NULL if an error occurs.
 * Result must be freed with mailbox_free_tree(Mailbox*).
 */
Mailbox* morse_client_get_mailboxes(MorseClient *client);


/*
 * Selects a mailbox
 */
ImapResponse* morse_client_select_box(MorseClient *client, const char *box_name);


/*
 * Statefully turns the client into IDLE mode
 * https://tools.ietf.org/html/rfc2177
 */
ImapResponse* morse_client_idle_on(MorseClient *client, 
                                   curl_debug_callback idle_callback);


#ifdef __cplusplus
}
#endif


#endif

