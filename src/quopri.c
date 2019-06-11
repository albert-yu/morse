#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dataconv.h"
#include "quopri.h"


#define ESCAPE_CHAR '=' 


char* quopri_decode(const char *input, int header) {
    char *decoded = NULL;
    if (!input) {
        return decoded; // NULL
    }
    size_t buf_size = 1;
    size_t len = 0;
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

    // tokenize by newlines
    char *saveptr = NULL;
    saveptr = strtok(cp_input, "\n");
    while (saveptr != NULL) {
        size_t i, n;
        i = 0;  // index
        n = strlen(saveptr);  // length of line
        while (i < n) {
            
        }
        saveptr = strtok(NULL, "\n");
    }

    if (cp_input) {
        free(cp_input);
        cp_input = NULL;
    }
    return decoded; 
}
