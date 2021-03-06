#ifndef SEND_H
#define SEND_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "curl/curl.h"

#include "endpoints.h"
#include "statuscodes.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX_FILE_EXT_CHAR_COUNT
#define MAX_FILE_EXT_CHAR_COUNT 4
#endif

typedef struct smtp_request_t {
    /* the recipients, comma-delimited */
    char *to;

    /* carbon copied, comma-delimited */ 
    char *cc;

    /* blind carbon copied, comma-delimited */
    char *bcc;

    /* subject */
    char *subject;

    /* body as string */
    char *body;

    /* the MIME type of the body (text/html, text/plain) */
    char *mimetype;

    /* CURL linked list of attachment file paths */
    struct curl_slist *attachments;
} SmtpRequest;

/**
 * Send an email to the designated recipients.
 * Recipients are comma-delimited strings.
 * @bearertoken - the token used for authentication
 * @user_email - the user's email address
 * @smtp_req - pointer to an SmtpRequest--unused fields must be NULL
 */
int morse_sendmail_google(char *bearertoken, char *user_email, SmtpRequest *smtp_req);

#ifdef __cplusplus
}
#endif


#endif

