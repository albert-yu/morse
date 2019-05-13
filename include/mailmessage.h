#ifndef MAILMESSAGE_H
#define MAILMESSAGE_H


/*
 * Metadata of the email message 
 * that is typically displayed 
 * without opening it.
 */
typedef struct mailmetadata_t {
    char *subject;
    char *from;
    char *to;
    char *cc;
    char *bcc;
    char *preview;
    int read;
} MailMetadata;


typedef struct mailmessage_t {
    size_t uid;
    MailMetadata *metadata;    
    char *body;
} MailMessage;

#endif