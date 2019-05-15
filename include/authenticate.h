#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "browser.h"
#include "server.h"
#include "strarray.h"
#include "hashtable.h"
#include "endpoints.h"

#ifdef __cplusplus
extern "C" {
#endif



/**
 * Gets authentication token and calls the appropriate validation
 * endpoint. 
 * Caller must free returned buffer.
 */
char* getgooglebearertoken();


/**
 * Gets user's gmail address. Caller must free returned buffer.
 */
char* getgmailaddress(char *token);


/**
 * Is this token valid?
 */
int isvalidtoken(char *token);

#ifdef __cplusplus
}
#endif

#endif