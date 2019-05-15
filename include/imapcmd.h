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
 * Retrieves all UIDs in a given mailbox.
 * SELECT {box} must be called first and 
 * the same CURL handle must be used.
 */
char *imapcmd_search_all_uids();

/*
 * Creates a folder.
 */
char *imapcmd_create_folder(char *foldername);


/*
 * Fetches the subject of the message WITHOUT opening it (marking
 * as read)
 */
char* imapcmd_id_get_subject(size_t id);


/*
 * Fetches the text of the message with the given ID
 */
char* imapcmd_id_fetch_body_text(size_t id);


/*
 * Lists the folders that are available.
 */
char* imapcmd_list_boxes();


/*
 * Fetches a range of messages with the starting index
 */
char* imapcmd_fetch_messages(size_t start_id, size_t length); 


/* 
 * Lists most recent messages.
 */
char *imapcmd_list_messages(size_t total_msg_count, size_t length);

#ifdef __cplusplus
}
#endif

#endif 