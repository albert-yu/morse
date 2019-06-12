#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dataconv.h"
#include "quopri.h"


#define ESCAPE_CHAR '=' 


/*
 * Is this character whitespace?
 */
int is_w_space(char c) {
    return (c == ' ' || c == '\r' || c == '\t');
}


/*
 * Implementation copied from:
 * https://github.com/python/cpython/blob/3.8/Lib/quopri.py#L117
 */
char* quopri_decode(const char *input, int header) {
    char *decoded = NULL;
    if (!input) {
        return decoded; // NULL
    }
    size_t buf_size = 1;
    decoded = malloc(buf_size * sizeof(*decoded));
    if (!decoded) {
        fprintf(stderr, "Could not alloc memory for decoded! (quopri)\n");
        return NULL;
    }

    // dup the string
    char *cp_input = strdup(input);
    if (!cp_input) {
        fprintf(stderr, "Could not dup string!\n");
        free(decoded);
        return NULL;
    }

    // we use this ptr for modification
    // of buffer
    char *decoded_ptr = decoded;

    // tokenize by newlines
    char *saveptr = NULL;
    saveptr = strtok(cp_input, "\n");
    size_t len = 0;  // length of string
    while (saveptr != NULL) {
        // check if need to realloc
        if (len == buf_size) {
            char *new_decoded = realloc(
                decoded,
                buf_size * 2);
            if (!new_decoded) {
                fprintf(stderr, 
                    "Not enough mem for realloc!\n");
                free(decoded);
                decoded = NULL;
                break;
            }

            // should have valid buffer
            decoded = new_decoded;
            buf_size *= 2;

            // move the ptr to the 
            // correct place
            decoded_ptr = decoded;
            decoded_ptr += len;
        }
        int partial = 1;
        size_t i, n;
        i = 0;  // index
        n = strlen(saveptr);  // length of line

        // check for partial
        if (n > 0 && saveptr[n - 1] == '\n') {
            partial = 0;
            n--;
            // strip trailing whitespace
            while (n > 0 && is_w_space(saveptr[n - 1])) {
                n--;
            }
        }
        // else: partial = 1, which it is already
        while (i < n) {
            char c = saveptr[i];
            if (c == '_' && header) {
                *decoded_ptr = ' ';
                i++;
            }
            else if (c != ESCAPE_CHAR) {
                *decoded_ptr = c;
                i++;
            }
            else if (((i + 1) == n) && !partial) {
                partial = 1;
                break;
            }
            else if (i + 1 < n && saveptr[i + 1] == ESCAPE_CHAR) {
                *decoded_ptr = ESCAPE_CHAR;
                i += 2;
            }
            else if (i + 2 < n &&
                     is_hex(saveptr[i + 1]) && is_hex(saveptr[i + 2])) {
                // hex to byte 
                char hex [3];
                hex[0] = saveptr[i + 1];
                hex[1] = saveptr[i + 2];
                hex[2] = '\0';
                unsigned char *ordinal = hex_to_bytes(hex);
                
                if (!ordinal) {
                    // eek!
                    fprintf(stderr,
                        "Could not get byte array from hex.\n");
                    break;
                }

                // byte array should be null-terminated
                // but its length should be 1 anyway
                *decoded_ptr = (char)*ordinal;
                free(ordinal); 

                i += 3;
            }
            else {
                // bad escape sequence -- leave as-is
                *decoded_ptr = c;
                i++;
            }

            decoded_ptr++;
            len++;
        }
        
        // if (!partial) {
        //     *decoded_ptr = '\n';
        //     decoded_ptr++;
        //     len++;
        // }
        saveptr = strtok(NULL, "\n");
    }

    if (cp_input) {
        free(cp_input);
        cp_input = NULL;
    }
    return decoded; 
}
