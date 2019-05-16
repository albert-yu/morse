#include <stdlib.h>
#include "mailmessage.h"

void mailmetadata_setdefault(MailMetadata *meta) {
    if (meta) {
        meta->subject = NULL;
        meta->from = NULL;
        meta->to = NULL;
        meta->cc = NULL;
        meta->bcc = NULL;
        meta->preview = NULL;
        meta->marked_read = 0;
    }
}


void mailmessage_setdefault(MailMessage *msg) {
    if (msg) {
        msg->uid = 0;
        msg->metadata = NULL;
        msg->body = NULL;
    }
}


void mailmetadata_free(MailMetadata *meta) {
    if (meta) {
        if (meta->subject){
            free(meta->subject);
        }
        if (meta->from) {
            free(meta->from);
        }
        if (meta->to) {
            free(meta->to);
        }
        if (meta->cc){
            free(meta->cc);
        }
        if (meta->bcc) {
            free(meta->bcc);
        }
        if (meta->preview) {
            free(meta->preview);
        }

        // set everything to NULL or 0
        mailmetadata_setdefault(meta);
        free(meta);
        meta = NULL;
    }
}


void mailmessage_free(MailMessage *msg) {
    if (msg) {
        if (msg->metadata) {
            mailmetadata_free(msg->metadata);
        }
        if (msg->body) {
            free(msg->body);
            msg->body = NULL;
        }
        // free(msg);
        // msg = NULL;
    }
}