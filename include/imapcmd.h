#ifndef IMAPCMD_H
#define IMAPCMD_H


#ifdef __cplusplus
extern "C" {
#endif

/*
 * This module contains functions for constructing IMAP commands.
 */

/*
 * Prepends "SELECT " to a given box name  
 */
char* imapcmd_select_box(char *box_name);


#ifdef __cplusplus
}
#endif

#endif 