#ifndef RECEIVE_H
#define RECEIVE_H

#include "curl/curl.h"
#include "endpoints.h"
#include "imap_response.h"
#include "statuscodes.h"


#ifdef __cplusplus
extern "C" {
#endif

/*
 * Invokes the authenticated process for the user
 */
CURL* get_imap_curl_google(void);

void list_last_n(char *box_name, size_t n);

ImapResponse* morse_exec_imap_stateful(CURL *curl, char *command);

#ifdef __cplusplus
}
#endif

#endif
