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
 * Creates a folder.
 */
char *imapcmd_create_folder(char *foldername);


/*
 * Fetches the subject of the message WITHOUT opening it (marking
 * as read)
 */
char* imapcmd_uid_get_subject(char *uid);


/*
 * Fetches the text of the message with the given UID
 */
char* imapcmd_uid_fetch_body_text(char *uid);


/*
 * Lists the folders that are available.
 */
char* imapcmd_list_boxes();


/* 
 * Lists most recent messages.
 */
char *imapcmd_list_messages(size_t total_msg_count, size_t length);

#ifdef __cplusplus
}
#endif

#endif 