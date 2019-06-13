#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "morse.h"
#include "receive.h" // not needed unless printing
#include "quopri.h"

int main(int argc, char *argv[]) {  
    MorseClient *client = morse_client_login(MailProvider_Google);
    char *email = NULL;
    if (argc == 2) {
        email = argv[1];
    }

    if (client) {
        if (email) {
            printf("Sending to: %s\n", email);
            SmtpRequest request;
            request.to = email;
            request.cc = NULL;
            request.bcc = NULL;
            request.subject = "Test email";
            request.body = "This is a test email.";
            request.mimetype = "text/plain";
            request.attachments = NULL;
            morse_client_sendmail(client, &request);
        }
        else {
            printf("Available mail boxes:\n");
            Mailbox *root = morse_client_get_mailboxes(client);
            if (root) {
                print_mailboxes(root);
                mailbox_free_tree(root);
            }

            printf("Selecting box...\n");
            ImapResponse *r = morse_client_select_box(client, "INBOX");
            if (r) {
                printf("%s\n", r->data->memory);
                imap_response_free(r);
            }
            
            // do some IMAP stuff
            char *imap_command = "FETCH 1 (UID RFC822.SIZE BODY.PEEK[])";
            printf("Executing IMAP command:\n");
            printf("------\n");
            printf("%s\n", imap_command);
            printf("------\n");
            ImapResponse *r2 = morse_client_exec_raw_imap(client, 
                imap_command, 1);
            if (r2) {
                // decode quoted-printable
                int headr = 1;
                char *decoded = quopri_decode(r2->v_data->memory, headr); 
                if (decoded) {
                    printf("Decoded:\n%s\n", decoded);
                    free(decoded);
                }
                // printf("%s", r2->v_data->memory);
                imap_response_free(r2);
            }
            
            // begin IDLE
            printf("Begin IDLE...\n");
            morse_client_begin_idle(client);
        }
        morse_client_logout(client);
    }
    return 0;
}

