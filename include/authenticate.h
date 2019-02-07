#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "browser.h"
#include "secrets.h"
#include "server.h"
#include "strarray.h"
#include "hashtable.h"

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
 * Is this token valid?
 */
int isvalidtoken(char *token);

#ifdef __cplusplus
}
#endif

#endif