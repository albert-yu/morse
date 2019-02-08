#ifndef SEND_H
#define SEND_H

// #include <cmark-gfm.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX_FILE_EXT_CHAR_COUNT
#define MAX_FILE_EXT_CHAR_COUNT 4
#endif


#define GOOGLE_SMTPS "smtps://smtp.gmail.com:465"
#define GOOGLE_SMTP "smtp://smtp.gmail.com:587"


/**
 * Send an email to the designated recipients.
 * Recipients are comma-delimited strings.
 * @to - the main recipient(s)
 * @cc - copied recipient(s)
 * @bcc - blind copied recipient(s)
 * @subject - email subject
 * @body - string of body
 * @mimetype - the MIME type of the body (text/html, text/plain)
 * @attachments - the file paths of the attachments, NULL-terminated
 */
int sendmail(char *to, char *cc, char *bcc, 
             char *subject, char *body, char *mimetype,
             char **attachments);


#ifdef __cplusplus
}
#endif


#endif