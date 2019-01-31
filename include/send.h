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


/**
 * Send an email to the designated recipients.
 * Recipients are semicolon-delimited strings.
 * @to - the main recipient(s)
 * @cc - copied recipient(s)
 * @bcc - blind copied recipient(s)
 * @subject - email subject
 * @body - string of body
 * @mimetype - the MIME type (text/html, text/plain)
 */
int sendmail(char *to, char *cc, char *bcc, 
             char *subject, 
             char *body, char *mimetype);


int test_smtp(void);

#ifdef __cplusplus
}
#endif


#endif