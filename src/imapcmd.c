#include <stdio.h>
#include <stdlib.h>

#include "imapcmd.h"



/*
 * Prepends "SELECT " to a given box name  
 */
char* imapcmd_select_box(char *box_name) {
    const char *template = "SELECT %s";
    char *buffer = calloc(64, sizeof(*buffer));
    sprintf(buffer, template, box_name);
    return buffer;
}






