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

#endif

