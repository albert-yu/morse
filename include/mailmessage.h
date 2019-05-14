#ifndef MAILMESSAGE_H
#define MAILMESSAGE_H

#ifdef __cplusplus
extern "C" {
#endif

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

    /* has this message been marked as read? */
    int marked_read;
} MailMetadata;


typedef struct mailmessage_t {
    size_t uid;
    MailMetadata *metadata;    
    char *body;
} MailMessage;


/*
 * Sets the default values for a struct. If 
 * a numeric-like value, then is 0. Otherwise,
 * a NULL pointer.
 */
void mailmetadata_setdefault(MailMetadata *meta);
void mailmessage_setdefault(MailMessage *msg);


/* 
 * Frees the struct and sets the pointer to NULL
 */
void mailmetadata_free(MailMetadata *meta);


/* 
 * Also frees the inner metadata member
 */
void mailmessage_free(MailMessage *msg);

#ifdef __cplusplus
}
#endif

#endif