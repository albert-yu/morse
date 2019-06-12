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
            else if (i + 1 < n && decoded_ptr[i + 1] == ESCAPE_CHAR) {
                *decoded_ptr = ESCAPE_CHAR;
                i += 2;
            }

            decoded_ptr++;
            len++;
        }
        saveptr = strtok(NULL, "\n");
    }

    if (cp_input) {
        free(cp_input);
        cp_input = NULL;
    }
    return decoded; 
}
