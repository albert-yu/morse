#include <stdlib.h>

#include "dataconv.h"
#include "quopri.h"


#define ESCAPE_CHAR_STR "=" 

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
    
    // get the lines
    char *cp_input = strdup(input);
    if (!cp_input) {
        fprintf(stderr, "Could not dup string!\n");
        free(decoded);
        return NULL;
    }

    char *saveptr = NULL;
    saveptr = strtok(cp_input, ESCAPE_CHAR_STR);
    while (saveptr != NULL) {
        
        saveptr = strtok(NULL, ESCAPE_CHAR_STR);
    }

    if (cp_input) {
        free(cp_input);
        cp_input = NULL;
    }
    return decoded; 
}
