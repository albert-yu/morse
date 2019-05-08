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


/*
 * Fetches the text of the message with the given UID
 */
char* imapcmd_uid_fetch_body_text(char *uid);


#ifdef __cplusplus
}
#endif

#endif 