/*
 * This is a module that should atomically 
 * generate tags that can be prefixed to
 * IMAP commands.
 */
#ifndef CMDTAG_H
#define CMDTAG_H


/*
 * Gets the next tag
 */
char* getnexttag(void);


/*
 * Prepends the command tag to a given IMAP 
 * command. E.g.
 * SELECT INBOX -> A001 SELECT INBOX
 * Returns NULL if failed.
 */
char* add_tag_to_cmd(const char *imap_cmd);


/*
 * Appends \r\n to the given command
 * Return NULL if mem alloc fails or
 * imap_cmd is too long
 */
char* add_carriage_ret(const char *imap_cmd);


#endif

