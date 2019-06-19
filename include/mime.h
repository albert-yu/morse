#ifndef _MIME_H_
#define _MIME_H_

#include "hashtable.h"


#ifdef __cplusplus
extern "C" {
#endif

/* MIME type constants */
#define MIME_TYPE_OCTET_STREAM "application/octet-stream"
#define MIME_TYPE_HTML "text/html"
#define MIME_TYPE_JPG "image/jpg"
#define MIME_TYPE_CSS "text/css"
#define MIME_TYPE_JS "application/javascript"
#define MIME_TYPE_JSON "application/json"
#define MIME_TYPE_PLAIN "text/plain"
#define MIME_TYPE_GIF "image/gif"
#define MIME_TYPE_PNG "image/png"
#define MIME_TYPE_MARKDOWN "text/markdown" // https://tools.ietf.org/html/rfc7763

extern char *mime_type_get(char *filename);


#ifdef __cplusplus
}
#endif

#endif

