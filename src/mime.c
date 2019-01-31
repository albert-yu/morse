#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "mime.h"

#define DEFAULT_MIME_TYPE MIME_TYPE_OCTET_STREAM

/**
 * Lowercase a string
 */
char* strlower(char *s) {
    for (char *p = s; *p != '\0'; p++) {       
        *p = tolower(*p);
    }
    return s;
}

/**
 * Lowercase a string and return 
 * a new one without modifying the
 * input. Caller must free the returned buffer.
 */
char* strlower_newstr(char *s) {
    char *new_s = calloc(strlen(s) + 1, sizeof(*new_s));
    char *new_p = new_s;
    for (char *p = s; *p; ++p) {
        *new_p = tolower(*p);
        ++new_p;
    }
    return new_s;
}

/**
 * Creates lookup. Returns NULL if key d.n.e. Must be freed.
 */
struct hashtable* create_mime_lookup() {
    const int HT_SIZE = 128;
    struct hashtable *ht = hashtable_create(HT_SIZE, NULL);
    hashtable_put(ht, "html", MIME_TYPE_HTML);
    hashtable_put(ht, "htm", MIME_TYPE_HTML);
    hashtable_put(ht, "jpeg", MIME_TYPE_JPG);
    hashtable_put(ht, "jpg", MIME_TYPE_JPG);
    hashtable_put(ht, "css", MIME_TYPE_CSS);
    hashtable_put(ht, "js", MIME_TYPE_JS);
    hashtable_put(ht, "json", MIME_TYPE_JSON);
    hashtable_put(ht, "txt", MIME_TYPE_PLAIN);
    hashtable_put(ht, "gif", MIME_TYPE_GIF);
    hashtable_put(ht, "png", MIME_TYPE_PNG);
    hashtable_put(ht, "md", MIME_TYPE_MARKDOWN);
    return ht;
}

/**
 * Return a MIME type for a given filename. Caller must free returned buffer.
 */
char* mime_type_get(char *filename) {
    char *ext = strrchr(filename, '.');
    // printf("%s\n", ext);
    if (ext == NULL) {
        return DEFAULT_MIME_TYPE;
    }
    ext++;

    // strlower causing error 
    char *ext_lowered = strlower_newstr(ext);
    struct hashtable *mime_lookup = create_mime_lookup();

    const size_t BUF_SIZE = 32; // long enough for the longest MIME type
    char *result = calloc(BUF_SIZE, sizeof(*result));
    void *value = hashtable_get(mime_lookup, ext_lowered);
    if (value) {
        char *chptr = (char*)value;
        strncpy(result, chptr, BUF_SIZE);
    }
    else {
        strncpy(result, DEFAULT_MIME_TYPE, BUF_SIZE);
    }

    free(ext_lowered);
    hashtable_destroy(mime_lookup);
    return result;
}

